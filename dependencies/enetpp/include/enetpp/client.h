#ifndef ENETPP_CLIENT_H_
#define ENETPP_CLIENT_H_

#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <assert.h>
#include "global_state.h"
#include "client_connect_params.h"
#include "client_queued_packet.h"
#include "client_statistics.h"
#include "set_current_thread_name.h"
#include "trace_handler.h"

namespace enetpp {
    enum connection_state {
        CONNECT_NONE,
        CONNECT_CONNECTING,
        CONNECT_FAILED,
        CONNECT_CONNECTED,
    };

	class client {
	private:
		trace_handler _trace_handler;

		std::queue<client_queued_packet> _packet_queue;
		std::mutex _packet_queue_mutex;

		std::queue<ENetEvent> _event_queue;
		std::queue<ENetEvent> _event_queue_copy; //member variable instead of stack variable to prevent mallocs?
		std::mutex _event_queue_mutex;

		bool _should_exit_thread;

		std::unique_ptr<std::thread> _thread;
        connection_state _connection_state{ CONNECT_NONE };
		client_statistics _statistics;

	public:
		client()
			: _should_exit_thread(false) {
		}

		~client() {
			//responsibility of owners to make sure disconnect is always called. not calling disconnect() in destructor due to
			//trace_handler side effects.
			assert(_thread == nullptr);
			assert(_packet_queue.empty());
			assert(_event_queue.empty());
			assert(_event_queue_copy.empty());
		}

		void set_trace_handler(trace_handler handler) {
			assert(!is_connecting_or_connected()); //must be set before any threads started to be safe
			_trace_handler = handler;
		}

		bool is_connecting_or_connected() const {
			return _thread != nullptr;
		}

        connection_state get_connection_state() const {
            return _connection_state;
        }

		void connect(const client_connect_params& params) {
			assert(global_state::get().is_initialized());
			assert(!is_connecting_or_connected());
			assert(params._channel_count > 0);
			assert(params._server_port != 0);
			assert(!params._server_host_name.empty());

			trace("connecting to '" + params._server_host_name + ":" + std::to_string(params._server_port) + "'");

            _connection_state = CONNECT_CONNECTING;

			_should_exit_thread = false;
			_thread = std::make_unique<std::thread>(&client::run_in_thread, this, params);
		}

		void disconnect() {
			if (_thread != nullptr) {
				_should_exit_thread = true;
				_thread->join();
				_thread.release();
			}

			destroy_all_queued_packets();
			destroy_all_queued_events();
		}

		void send_packet(enet_uint8 channel_id, const enet_uint8* data, size_t data_size, enet_uint32 flags) {
			assert(is_connecting_or_connected());
			if (_thread != nullptr) {
				std::lock_guard<std::mutex> lock(_packet_queue_mutex);
				auto packet = enet_packet_create(data, data_size, flags);
				_packet_queue.emplace(channel_id, packet);
			}
		}

		void consume_events(
			std::function<void()> on_connected,
			std::function<void()> on_disconnected,
			std::function<void(const enet_uint8* data, size_t data_size)> on_data_received) {

			if (!_event_queue.empty()) {

				//!IMPORTANT! neet to copy the events for consumption to prevent deadlocks!
				//ex.
				//- event = JoinGameFailed packet received
				//- causes event_handler to call disconnect
				//- disconnect deadlocks as the thread needs a critical section on events to exit
				{
					std::lock_guard<std::mutex> lock(_event_queue_mutex);
					assert(_event_queue_copy.empty());
					_event_queue_copy = _event_queue;
					_event_queue = {};
				}

				bool is_disconnected = false;

				while (!_event_queue_copy.empty()) {
					auto& e = _event_queue_copy.front();
					switch (e.type) {
						case ENET_EVENT_TYPE_CONNECT: {
							on_connected();
							break;
						}

						case ENET_EVENT_TYPE_DISCONNECT: {
							on_disconnected();
							is_disconnected = true;
							break;
						}

						case ENET_EVENT_TYPE_RECEIVE: {
							on_data_received(e.packet->data, e.packet->dataLength);
							enet_packet_destroy(e.packet);
							break;
						}

						case ENET_EVENT_TYPE_NONE:
						default:
							assert(false);
							break;
					}
					_event_queue_copy.pop();
				}

				if (is_disconnected) {
					//cleanup everything internally, make sure the thread is cleaned up.
					disconnect();
				}
			}
		}

		const client_statistics& get_statistics() const {
			return _statistics;
		}

	private:
		void destroy_all_queued_packets() {
			std::lock_guard<std::mutex> lock(_packet_queue_mutex);
			while (!_packet_queue.empty()) {
				enet_packet_destroy(_packet_queue.front()._packet);
				_packet_queue.pop();
			}
		}

		void destroy_all_queued_events() {
			std::lock_guard<std::mutex> lock(_event_queue_mutex);
			while (!_event_queue.empty()) {
				destroy_unhandled_event_data(_event_queue.front());
				_event_queue.pop();
			}
		}

		void destroy_unhandled_event_data(ENetEvent& e) {
			if (e.type == ENET_EVENT_TYPE_RECEIVE) {
				enet_packet_destroy(e.packet);
			}
		}

		void run_in_thread(const client_connect_params& params) {
            trace("in thread");
			set_current_thread_name("enetpp::client");

			ENetHost* host = enet_host_create(nullptr, 1, params._channel_count, params._incoming_bandwidth, params._outgoing_bandwidth);
			if (host == nullptr) {
                _connection_state = CONNECT_FAILED;
				trace("enet_host_create failed");
				return;
			}

			auto address = params.make_server_address();
			ENetPeer* peer = enet_host_connect(host, &address, params._channel_count, 0);
			if (peer == nullptr) {
                _connection_state = CONNECT_FAILED;
				trace("enet_host_connect failed");
				enet_host_destroy(host);
				return;
			}

			enet_uint32 enet_timeout = static_cast<enet_uint32>(params._timeout.count());
			enet_peer_timeout(peer, 0, enet_timeout, enet_timeout);

			bool is_disconnecting = false;
			enet_uint32 disconnect_start_time = 0;

			while (peer != nullptr) {
				_statistics._round_trip_time_in_ms = peer->roundTripTime;
				_statistics._round_trip_time_variance_in_ms = peer->roundTripTimeVariance;

				if (_should_exit_thread) {
					if (!is_disconnecting) {
						enet_peer_disconnect(peer, 0);
						is_disconnecting = true;
						disconnect_start_time = enet_time_get();
					}
					else {
						if ((enet_time_get() - disconnect_start_time) > 1000) {
							trace("enet_peer_disconnect took too long");
							enet_peer_reset(peer);
							peer = nullptr;
							break;
						}
					}
				}

				if (!is_disconnecting) {
					send_queued_packets_in_thread(peer);
				}

				//flush / capture enet events
				//http://lists.cubik.org/pipermail/enet-discuss/2013-September/002240.html
				enet_host_service(host, 0, 0);
				{
					ENetEvent e;
					while (enet_host_check_events(host, &e) > 0) {
						std::lock_guard<std::mutex> lock(_event_queue_mutex);
						_event_queue.push(e);

                        if (e.type == ENET_EVENT_TYPE_CONNECT) {
                            _connection_state = CONNECT_CONNECTED;
                        }

						if (e.type == ENET_EVENT_TYPE_DISCONNECT) {
							trace("ENET_EVENT_TYPE_DISCONNECT received");
							peer = nullptr;
							break;
						}
					}
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

            trace("leaving thread");
            _connection_state = CONNECT_FAILED;

			enet_host_destroy(host);
		}

		void send_queued_packets_in_thread(ENetPeer* peer) {
			if (!_packet_queue.empty()) {
				std::lock_guard<std::mutex> lock(_packet_queue_mutex);
				while (!_packet_queue.empty()) {
					auto qp = _packet_queue.front();
					_packet_queue.pop();

					if (enet_peer_send(peer, qp._channel_id, qp._packet) != 0) {
						trace("enet_peer_send failed");
					}

					if (qp._packet->referenceCount == 0) {
						enet_packet_destroy(qp._packet);
					}
				}
			}
		}

		void trace(const std::string& s) {
			if (_trace_handler != nullptr) {
				_trace_handler(s);
			}
		}
	};

}

#endif