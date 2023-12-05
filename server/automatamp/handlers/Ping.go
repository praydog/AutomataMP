package handlers

import (
	"github.com/codecat/go-enet"
	"github.com/codecat/go-libs/log"
	core "github.com/praydog/AutomataMP/server/automatamp/core"
	nier "github.com/praydog/AutomataMP/server/automatamp/nier"
	structs "github.com/praydog/AutomataMP/server/automatamp/structs"
)

func HandlePing(sender enet.Peer, connection *structs.Connection) {
	log.Info("Ping received from %s", connection.Client.Name)
	sender.SendBytes(core.MakeEmptyPacketBytes(nier.PacketTypeID_PONG), 0, enet.PacketFlagReliable)
}
