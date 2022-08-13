package automatamp

import (
	nier "github.com/praydog/AutomataMP/server/automatamp/nier"

	"github.com/codecat/go-libs/log"
	flatbuffers "github.com/google/flatbuffers/go"
)

func checkValidPacket(data *nier.Packet) bool {
	// recovering from the panic will return false
	// so this should be fine
	// the reason for the panic handler is so some client
	// can't send us garbage data to crash the server.
	defer handlepanic()

	if data.Magic() != 1347240270 {
		log.Error("Invalid magic number: %d", data.Magic())
		return false
	}

	if data.Id() == 0 {
		log.Error("Invalid packet type: %d", data.Id())
		return false
	}

	return true
}

func packetStart(id nier.PacketType) *flatbuffers.Builder {
	builder := flatbuffers.NewBuilder(0)
	nier.PacketStart(builder)
	nier.PacketAddMagic(builder, 1347240270)
	nier.PacketAddId(builder, id)
	//nier.PacketEnd(builder)

	return builder
}

func makeVectorData(builder *flatbuffers.Builder, data []uint8) flatbuffers.UOffsetT {
	dataoffs := flatbuffers.UOffsetT(0)

	if len(data) > 0 {
		nier.PacketStartDataVector(builder, len(data))
		for i := len(data) - 1; i >= 0; i-- {
			builder.PrependUint8(data[i])
		}
		dataoffs = builder.EndVector(len(data))
	}

	return dataoffs
}

func packetStartWithData(id nier.PacketType, data []uint8) *flatbuffers.Builder {
	builder := flatbuffers.NewBuilder(0)

	dataoffs := makeVectorData(builder, data)

	nier.PacketStart(builder)
	nier.PacketAddMagic(builder, 1347240270)
	nier.PacketAddId(builder, id)

	if (len(data)) > 0 {
		nier.PacketAddData(builder, dataoffs)
	}
	//nier.PacketEnd(builder)

	return builder
}

func makePacketBytes(id nier.PacketType, data []uint8) []uint8 {
	builder := packetStartWithData(id, data)
	builder.Finish(nier.PacketEnd(builder))
	return builder.FinishedBytes()
}

func makeEmptyPacketBytes(id nier.PacketType) []uint8 {
	builder := packetStart(id)
	builder.Finish(nier.PacketEnd(builder))
	return builder.FinishedBytes()
}

func builderSurround(cb func(*flatbuffers.Builder) flatbuffers.UOffsetT) []uint8 {
	builder := flatbuffers.NewBuilder(0)
	offs := cb(builder)
	builder.Finish(offs)
	return builder.FinishedBytes()
}

func makePlayerPacketBytes(connection *Connection, id nier.PacketType, data []uint8) []uint8 {
	playerPacketData := builderSurround(func(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
		dataoffs := makeVectorData(builder, data)

		nier.PlayerPacketStart(builder)
		nier.PlayerPacketAddGuid(builder, connection.client.guid)
		nier.PlayerPacketAddData(builder, dataoffs)
		return nier.PlayerPacketEnd(builder)
	})

	return makePacketBytes(id, playerPacketData)
}

func makeEntityPacketBytes(guid uint32, id nier.PacketType, data []uint8) []uint8 {
	entityPacketData := builderSurround(func(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
		dataoffs := makeVectorData(builder, data)

		nier.EntityPacketStart(builder)
		nier.EntityPacketAddGuid(builder, guid)
		nier.EntityPacketAddData(builder, dataoffs)
		return nier.EntityPacketEnd(builder)
	})

	return makePacketBytes(id, entityPacketData)
}
