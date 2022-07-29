#ifndef ENETPP_CLIENT_QUEUED_PACKET_H_
#define ENETPP_CLIENT_QUEUED_PACKET_H_

#include "enet/enet.h"

namespace enetpp {

	class client_queued_packet {
	public:
		enet_uint8 _channel_id;
		ENetPacket* _packet;

	public:
		client_queued_packet()
			: _channel_id(0)
			, _packet(nullptr) {
		}

		client_queued_packet(enet_uint8 channel_id, ENetPacket* packet)
			: _channel_id(channel_id)
			, _packet(packet) {
		}
	};

}

#endif