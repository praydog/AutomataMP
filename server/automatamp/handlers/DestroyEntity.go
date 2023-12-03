package handlers

import (
	"github.com/codecat/go-enet"
	"github.com/codecat/go-libs/log"
	flatbuffers "github.com/google/flatbuffers/go"

	core "github.com/praydog/AutomataMP/server/automatamp/core"
	nier "github.com/praydog/AutomataMP/server/automatamp/nier"
	structs "github.com/praydog/AutomataMP/server/automatamp/structs"
)

func HandleDestroyEntity(server *structs.Server, sender enet.Peer, connection *structs.Connection, data *nier.Packet) {
	log.Info("Destroy entity received")
	if !connection.Client.IsMasterClient {
		log.Info(" Not a master client, ignoring")
		return
	}

	// Destroy the entity.
	entityPkt := &nier.EntityPacket{}
	flatbuffers.GetRootAs(data.DataBytes(), 0, entityPkt)

	delete(server.Entities, entityPkt.Guid())

	core.BroadcastPacketToAllExceptSender(server, sender, nier.PacketTypeID_DESTROY_ENTITY, data.DataBytes())
}
