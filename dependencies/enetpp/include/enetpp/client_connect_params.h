#ifndef ENETPP_CLIENT_CONNECT_PARAMS_H_
#define ENETPP_CLIENT_CONNECT_PARAMS_H_

#include "enet/enet.h"
#include <chrono>

namespace enetpp {

	class client_connect_params {
	public:
		size_t _channel_count;
		enet_uint32 _incoming_bandwidth;
		enet_uint32 _outgoing_bandwidth;
		std::string _server_host_name;
		enet_uint16 _server_port;
		std::chrono::milliseconds _timeout;

	public:
		client_connect_params() 
			: _channel_count(0)
			, _incoming_bandwidth(0)
			, _outgoing_bandwidth(0)
			, _server_host_name()
			, _server_port(0)
			, _timeout(0) {
		}

		client_connect_params& set_channel_count(size_t channel_count) {
			_channel_count = channel_count;
			return *this;
		}

		client_connect_params& set_incoming_bandwidth(enet_uint32 bandwidth) {
			_incoming_bandwidth = bandwidth;
			return *this;
		}

		client_connect_params& set_outgoing_bandwidth(enet_uint32 bandwidth) {
			_outgoing_bandwidth = bandwidth;
			return *this;
		}

		client_connect_params& set_server_host_name_and_port(const char* host_name, enet_uint16 port) {
			_server_host_name = host_name;
			_server_port = port;
			return *this;
		}

		client_connect_params& set_timeout(std::chrono::milliseconds timeout) {
			_timeout = timeout;
			return *this;
		}

		ENetAddress make_server_address() const {
			ENetAddress address;
			enet_address_set_host(&address, _server_host_name.c_str());
			address.port = _server_port;
			return address;
		}

	};

}

#endif