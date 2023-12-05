package handlers

import (
	"github.com/codecat/go-enet"
	"github.com/codecat/go-libs/log"
	flatbuffers "github.com/google/flatbuffers/go"
	core "github.com/praydog/AutomataMP/server/automatamp/core"
	nier "github.com/praydog/AutomataMP/server/automatamp/nier"
	structs "github.com/praydog/AutomataMP/server/automatamp/structs"
)

func HandleEntityAnimationStart(server *structs.Server, sender enet.Peer, connection *structs.Connection, data *nier.Packet) {
	log.Info("ENTITY Animation start received")

	if !connection.Client.IsMasterClient {
		log.Info(" Not a master client, ignoring")
		return
	}

	entityPkt := &nier.EntityPacket{}
	flatbuffers.GetRootAs(data.DataBytes(), 0, entityPkt)

	animationData := &nier.AnimationStart{}
	flatbuffers.GetRootAs(entityPkt.DataBytes(), 0, animationData)

	// log.Info(" Animation: %d", animationData.Anim())
	// log.Info(" Variant: %d", animationData.Variant())
	// log.Info(" a3: %d", animationData.A3())
	// log.Info(" a4: %d", animationData.A4())

	// TODO: sanitize the data

	// Broadcast the packet back to all valid clients (except the sender)
	core.BroadcastPacketToAllExceptSender(server, sender, nier.PacketTypeID_ENTITY_ANIMATION_START, data.DataBytes())
}
