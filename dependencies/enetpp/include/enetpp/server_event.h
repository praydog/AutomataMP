#ifndef ENETPP_SERVER_EVENT_H_
#define ENETPP_SERVER_EVENT_H_

#include "enet/enet.h"

namespace enetpp {

	//server can't use ENetEvent as ENetPeer is not thread safe. Instead track data that is safe.
	template<typename ClientT>
	class server_event {
	public:
		ENetEventType _event_type;
		enet_uint8 _channel_id;
		ENetPacket* _packet;
		ClientT* _client;
	
	public:
		server_event()
			: _event_type(ENET_EVENT_TYPE_NONE)
			, _channel_id(0)
			, _packet(nullptr)
			, _client(nullptr) {
		}

		server_event(ENetEventType event_type, enet_uint8 channel_id, ENetPacket* packet, ClientT* client)
			: _event_type(event_type)
			, _channel_id(channel_id)
			, _packet(packet)
			, _client(client) {
		}
	};

}

#endif
