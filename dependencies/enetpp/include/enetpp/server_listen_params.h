#ifndef ENETPP_SERVER_LISTEN_PARAMS_H_
#define ENETPP_SERVER_LISTEN_PARAMS_H_

#include "enet/enet.h"
#include <chrono>
#include <functional>

namespace enetpp {

	template<typename ClientT>
	class server_listen_params {
	public:
		using initialize_client_function = std::function<void(ClientT& client, const char* ip)>;

	public:
		size_t _max_client_count;
		size_t _channel_count;
		enet_uint32 _incoming_bandwidth;
		enet_uint32 _outgoing_bandwidth;
		enet_uint16 _listen_port;
		std::chrono::milliseconds _peer_timeout;
		initialize_client_function _initialize_client_function;

	public:
		server_listen_params()
			: _max_client_count(0)
			, _channel_count(0)
			, _incoming_bandwidth(0)
			, _outgoing_bandwidth(0) 
			, _peer_timeout(0) {
		}

		server_listen_params& set_listen_port(enet_uint16 port) {
			_listen_port = port;
			return *this;
		}

		server_listen_params& set_max_client_count(size_t count) {
			_max_client_count = count;
			return *this;
		}

		server_listen_params& set_channel_count(size_t channel_count) {
			_channel_count = channel_count;
			return *this;
		}

		server_listen_params& set_incoming_bandwidth(enet_uint32 bandwidth) {
			_incoming_bandwidth = bandwidth;
			return *this;
		}

		server_listen_params& set_outgoing_bandwidth(enet_uint32 bandwidth) {
			_outgoing_bandwidth = bandwidth;
			return *this;
		}

		server_listen_params& set_peer_timeout(std::chrono::milliseconds timeout) {
			_peer_timeout = timeout;
			return *this;
		}

		server_listen_params& set_initialize_client_function(initialize_client_function f) {
			_initialize_client_function = f;
			return *this;
		}		

		ENetAddress make_listen_address() const {
			ENetAddress address;
			address.host = ENET_HOST_ANY;
			address.port = _listen_port;
			return address;
		}
	};

}

#endif
