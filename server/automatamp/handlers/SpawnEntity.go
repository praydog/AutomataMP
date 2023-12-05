package handlers

import (
	"github.com/codecat/go-enet"
	"github.com/codecat/go-libs/log"
	flatbuffers "github.com/google/flatbuffers/go"
	core "github.com/praydog/AutomataMP/server/automatamp/core"
	nier "github.com/praydog/AutomataMP/server/automatamp/nier"
	structs "github.com/praydog/AutomataMP/server/automatamp/structs"
)

func HandleSpawnEntity(server *structs.Server, sender enet.Peer, connection *structs.Connection, data *nier.Packet) {
	log.Info("Spawn entity received")
	if !connection.Client.IsMasterClient {
		log.Info(" Not a master client, ignoring")
		return
	}

	// Cache the entity.
	entityPkt := &nier.EntityPacket{}
	flatbuffers.GetRootAs(data.DataBytes(), 0, entityPkt)

	spawnInfo := &nier.EntitySpawnParams{}
	flatbuffers.GetRootAs(entityPkt.DataBytes(), 0, spawnInfo)

	if entityPkt.Guid() > server.HighestEntityGuid {
		server.HighestEntityGuid = entityPkt.Guid()
	}

	server.Entities[entityPkt.Guid()] = new(structs.ActiveEntity)
	server.Entities[entityPkt.Guid()].Guid = entityPkt.Guid()
	server.Entities[entityPkt.Guid()].SpawnInfo = spawnInfo

	core.BroadcastPacketToAllExceptSender(server, sender, nier.PacketTypeID_SPAWN_ENTITY, data.DataBytes())
}
