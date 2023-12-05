package handlers

import (
	core "github.com/praydog/AutomataMP/server/automatamp/core"
	nier "github.com/praydog/AutomataMP/server/automatamp/nier"
	structs "github.com/praydog/AutomataMP/server/automatamp/structs"

	"github.com/codecat/go-enet"
	"github.com/codecat/go-libs/log"
	flatbuffers "github.com/google/flatbuffers/go"
)

func HandleAnimationStart(server *structs.Server, sender enet.Peer, connection *structs.Connection, data *nier.Packet) {
	log.Info("Animation start received")

	animationData := &nier.AnimationStart{}
	flatbuffers.GetRootAs(data.DataBytes(), 0, animationData)

	// log.Info(" Animation: %d", animationData.Anim())
	// log.Info(" Variant: %d", animationData.Variant())
	// log.Info(" a3: %d", animationData.A3())
	// log.Info(" a4: %d", animationData.A4())

	// TODO: sanitize the data

	// Broadcast the packet back to all valid clients (except the sender)
	core.BroadcastPlayerPacketToAllExceptSender(server, sender, connection, nier.PacketTypeID_ANIMATION_START, data.DataBytes())
}
