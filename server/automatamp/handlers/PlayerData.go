package handlers

import (
	"github.com/codecat/go-enet"
	flatbuffers "github.com/google/flatbuffers/go"
	core "github.com/praydog/AutomataMP/server/automatamp/core"
	nier "github.com/praydog/AutomataMP/server/automatamp/nier"
	structs "github.com/praydog/AutomataMP/server/automatamp/structs"
)

func HandlePlayerData(server *structs.Server, sender enet.Peer, connection *structs.Connection, data *nier.Packet) {
	playerData := &nier.PlayerData{}
	flatbuffers.GetRootAs(data.DataBytes(), 0, playerData)

	connection.Client.LastPlayerData = playerData

	// Broadcast the packet back to all valid clients (except the sender)
	core.BroadcastPlayerPacketToAllExceptSender(server, sender, connection, nier.PacketTypeID_PLAYER_DATA, data.DataBytes())
}
