package handlers

import (
	"github.com/codecat/go-libs/log"
	flatbuffers "github.com/google/flatbuffers/go"
	core "github.com/praydog/AutomataMP/server/automatamp/core"
	nier "github.com/praydog/AutomataMP/server/automatamp/nier"
	structs "github.com/praydog/AutomataMP/server/automatamp/structs"
)

func HandleDestroyPlayer(server *structs.Server, connection *structs.Connection) {
	log.Info("Destroy Player received")
	destroyPlayerBytes := core.BuilderSurround(func(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
		return nier.CreateDestroyPlayer(builder, connection.Client.Guid)
	})

	core.BroadcastPacketToAllExceptSender(server, connection.Peer, nier.PacketTypeID_DESTROY_PLAYER, destroyPlayerBytes)
}
