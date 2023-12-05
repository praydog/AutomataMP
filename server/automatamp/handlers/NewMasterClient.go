package handlers

import (
	"github.com/codecat/go-enet"
	"github.com/codecat/go-libs/log"
	core "github.com/praydog/AutomataMP/server/automatamp/core"
	nier "github.com/praydog/AutomataMP/server/automatamp/nier"
	structs "github.com/praydog/AutomataMP/server/automatamp/structs"
)

func HandleNewMasterClient(server *structs.Server) {
	log.Info("NewMasterClient received")
	for conn, client := range server.Clients {
		log.Info("Setting new master client: %s @ %s", client.Name, conn.Peer.GetAddress())

		client.IsMasterClient = true
		conn.Peer.SendBytes(core.MakeEmptyPacketBytes(nier.PacketTypeID_SET_MASTER_CLIENT), 0, enet.PacketFlagReliable)
		break
	}
}
