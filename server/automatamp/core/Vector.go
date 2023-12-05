package core

import (
	nier "github.com/praydog/AutomataMP/server/automatamp/nier"

	flatbuffers "github.com/google/flatbuffers/go"
)

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
