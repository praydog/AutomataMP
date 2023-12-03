package core

import (
	"strconv"

	nier "github.com/praydog/AutomataMP/server/automatamp/nier"
	structs "github.com/praydog/AutomataMP/server/automatamp/structs"

	"github.com/codecat/go-enet"
	"github.com/codecat/go-libs/log"
)

func GetFilteredPlayerName(server *structs.Server, input []uint8) string {
	out := ""

	if input == nil || string(input) == "" {
		log.Error("Client sent empty name, assigning random name")
		out = "Client" + strconv.FormatInt(int64(len(server.Connections)), 10)
	} else {
		out = string(input)
	}

	for _, client := range server.Clients {
		if client.Name == out {
			log.Error("Client sent duplicate name, appending number")
			out = out + strconv.FormatInt(int64(len(server.Connections)), 10)
			break
		}
	}

	return out
}

func BroadcastPacketToAll(server *structs.Server, id nier.PacketType, data []uint8) {
	broadcastData := MakePacketBytes(id, data)
	for conn := range server.Clients {
		conn.Peer.SendBytes(broadcastData, 0, enet.PacketFlagReliable)
	}
}

func BroadcastPacketToAllExceptSender(server *structs.Server, sender enet.Peer, id nier.PacketType, data []uint8) {
	broadcastData := MakePacketBytes(id, data)
	for conn := range server.Clients {
		if conn.Peer == sender {
			continue
		}

		conn.Peer.SendBytes(broadcastData, 0, enet.PacketFlagReliable)
	}
}

func BroadcastPlayerPacketToAll(server *structs.Server, connection *structs.Connection, id nier.PacketType, data []uint8) {
	broadcastData := MakePlayerPacketBytes(connection.Client.Guid, id, data)

	for conn := range server.Clients {
		conn.Peer.SendBytes(broadcastData, 0, enet.PacketFlagReliable)
	}
}

func BroadcastPlayerPacketToAllExceptSender(server *structs.Server, sender enet.Peer, connection *structs.Connection, id nier.PacketType, data []uint8) {
	broadcastData := MakePlayerPacketBytes(connection.Client.Guid, id, data)

	for conn := range server.Clients {
		if conn.Peer == sender {
			continue
		}

		conn.Peer.SendBytes(broadcastData, 0, enet.PacketFlagReliable)
	}
}
