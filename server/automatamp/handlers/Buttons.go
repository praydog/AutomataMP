package handlers

import (
	core "github.com/praydog/AutomataMP/server/automatamp/core"
	nier "github.com/praydog/AutomataMP/server/automatamp/nier"
	structs "github.com/praydog/AutomataMP/server/automatamp/structs"

	"github.com/codecat/go-enet"
	"github.com/codecat/go-libs/log"
)

func HandleButtons(server *structs.Server, sender enet.Peer, connection *structs.Connection, data *nier.Packet) {
	log.Info("Buttons received")

	// Broadcast the packet back to all valid clients (except the sender)
	core.BroadcastPlayerPacketToAllExceptSender(server, sender, connection, nier.PacketTypeID_BUTTONS, data.DataBytes())
}
