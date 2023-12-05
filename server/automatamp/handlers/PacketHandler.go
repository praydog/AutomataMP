package handlers

import (
	"github.com/codecat/go-enet"
	"github.com/codecat/go-libs/log"
	nier "github.com/praydog/AutomataMP/server/automatamp/nier"
	structs "github.com/praydog/AutomataMP/server/automatamp/structs"
)

func PacketHandler(server *structs.Server, connection *structs.Connection, sender enet.Peer, data []byte) {
	packetData := nier.GetRootAsPacket(data, 0)

	switch packetData.Id() {
	case nier.PacketTypeID_HELLO:
		HandleHello(server, sender, connection, packetData)
	case nier.PacketTypeID_PING:
		HandlePing(sender, connection)
	case nier.PacketTypeID_PLAYER_DATA:
		HandlePlayerData(server, sender, connection, packetData)
	case nier.PacketTypeID_ANIMATION_START:
		HandleAnimationStart(server, sender, connection, packetData)
	case nier.PacketTypeID_BUTTONS:
		HandleButtons(server, sender, connection, packetData)
	case nier.PacketTypeID_SPAWN_ENTITY:
		HandleSpawnEntity(server, sender, connection, packetData)
	case nier.PacketTypeID_DESTROY_ENTITY:
		HandleDestroyEntity(server, sender, connection, packetData)
	case nier.PacketTypeID_ENTITY_DATA:
		HandleEntityData(server, sender, connection, packetData)
	case nier.PacketTypeID_ENTITY_ANIMATION_START:
		HandleEntityAnimationStart(server, sender, connection, packetData)
	default:
		log.Error("Unknown packet type: %d", packetData.Id())
	}
}
