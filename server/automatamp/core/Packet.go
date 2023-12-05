package core

import (
	nier "github.com/praydog/AutomataMP/server/automatamp/nier"

	"github.com/codecat/go-libs/log"
	flatbuffers "github.com/google/flatbuffers/go"
)

func CheckValidPacket(data *nier.Packet) bool {
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

func PacketStart(id nier.PacketType) *flatbuffers.Builder {
	builder := flatbuffers.NewBuilder(0)
	nier.PacketStart(builder)
	nier.PacketAddMagic(builder, 1347240270)
	nier.PacketAddId(builder, id)
	//nier.PacketEnd(builder)

	return builder
}

func PacketStartWithData(id nier.PacketType, data []uint8) *flatbuffers.Builder {
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

func MakePacketBytes(id nier.PacketType, data []uint8) []uint8 {
	builder := PacketStartWithData(id, data)
	builder.Finish(nier.PacketEnd(builder))
	return builder.FinishedBytes()
}

func MakeEmptyPacketBytes(id nier.PacketType) []uint8 {
	builder := PacketStart(id)
	builder.Finish(nier.PacketEnd(builder))
	return builder.FinishedBytes()
}

func BuilderSurround(cb func(*flatbuffers.Builder) flatbuffers.UOffsetT) []uint8 {
	builder := flatbuffers.NewBuilder(0)
	offs := cb(builder)
	builder.Finish(offs)
	return builder.FinishedBytes()
}

func MakePlayerPacketBytes(guid uint64, id nier.PacketType, data []uint8) []uint8 {
	playerPacketData := BuilderSurround(func(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
		dataoffs := makeVectorData(builder, data)

		nier.PlayerPacketStart(builder)
		nier.PlayerPacketAddGuid(builder, guid)
		nier.PlayerPacketAddData(builder, dataoffs)
		return nier.PlayerPacketEnd(builder)
	})

	return MakePacketBytes(id, playerPacketData)
}

func MakeEntityPacketBytes(guid uint32, id nier.PacketType, data []uint8) []uint8 {
	entityPacketData := BuilderSurround(func(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
		dataoffs := makeVectorData(builder, data)

		nier.EntityPacketStart(builder)
		nier.EntityPacketAddGuid(builder, guid)
		nier.EntityPacketAddData(builder, dataoffs)
		return nier.EntityPacketEnd(builder)
	})

	return MakePacketBytes(id, entityPacketData)
}
