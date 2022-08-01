package main

import (
	"automatampserver/Nier"

	"github.com/codecat/go-enet"
	"github.com/codecat/go-libs/log"
	flatbuffers "github.com/google/flatbuffers/go"
)

func check(err error) {
	if err != nil {
		panic(err)
	}
}

func checkValidPacket(data *Nier.Packet) bool {
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

func packetStart(id Nier.PacketType) *flatbuffers.Builder {
	builder := flatbuffers.NewBuilder(0)
	Nier.PacketStart(builder)
	Nier.PacketAddMagic(builder, 1347240270)
	Nier.PacketAddId(builder, id)
	//Nier.PacketEnd(builder)

	return builder
}

func packetStartWithData(id Nier.PacketType, data []uint8) *flatbuffers.Builder {
	builder := flatbuffers.NewBuilder(0)

	dataoffs := flatbuffers.UOffsetT(0)

	if len(data) > 0 {
		Nier.PacketStartDataVector(builder, len(data))
		for i := len(data) - 1; i >= 0; i-- {
			builder.PrependUint8(data[i])
		}
		dataoffs = builder.EndVector(len(data))
	}

	Nier.PacketStart(builder)
	Nier.PacketAddMagic(builder, 1347240270)
	Nier.PacketAddId(builder, id)

	if (len(data)) > 0 {
		Nier.PacketAddData(builder, dataoffs)
	}
	//Nier.PacketEnd(builder)

	return builder
}

func makePacketBytes(id Nier.PacketType, data []uint8) []uint8 {
	builder := packetStartWithData(id, data)
	builder.Finish(Nier.PacketEnd(builder))
	return builder.FinishedBytes()
}

func makeEmptyPacketBytes(id Nier.PacketType) []uint8 {
	builder := packetStart(id)
	builder.Finish(Nier.PacketEnd(builder))
	return builder.FinishedBytes()
}

func main() {
	// Initialize enet
	enet.Initialize()

	// Create a host listening on 0.0.0.0:6969
	host, err := enet.NewHost(enet.NewListenAddress(6969), 32, 1, 0, 0)
	if err != nil {
		log.Error("Couldn't create host: %s", err.Error())
		return
	}

	log.Info("Created host")

	// The event loop
	for true {
		// Wait until the next event
		ev := host.Service(1000)

		// Do nothing if we didn't get any event
		if ev.GetType() == enet.EventNone {
			continue
		}

		switch ev.GetType() {
		case enet.EventConnect: // A new peer has connected
			log.Info("New peer connected: %s", ev.GetPeer().GetAddress())
			break

		case enet.EventDisconnect: // A connected peer has disconnected
			log.Info("Peer disconnected: %s", ev.GetPeer().GetAddress())
			break

		case enet.EventReceive: // A peer sent us some data
			// Get the packet
			packet := ev.GetPacket()

			// We must destroy the packet when we're done with it
			defer packet.Destroy()

			// Get the bytes in the packet
			packetBytes := packet.GetData()
			log.Info("Peer sent data %d bytes", len(packetBytes))

			data := Nier.GetRootAsPacket(packetBytes, 0)

			if !checkValidPacket(data) {
				continue
			}

			switch data.Id() {
			case Nier.PacketTypeID_PING:
				log.Info("Ping received")

				ev.GetPeer().SendBytes(makeEmptyPacketBytes(Nier.PacketTypeID_PONG), 0, enet.PacketFlagReliable)
				break
			case Nier.PacketTypeID_PLAYER_DATA:
				log.Info("Player data received %d", flatbuffers.GetSizePrefix(data.DataBytes(), 0))
				playerData := &Nier.PlayerData{}
				flatbuffers.GetRootAs(data.DataBytes(), 0, playerData)

				log.Info("Flashlight: %d", playerData.Flashlight())
				log.Info("Speed: %f", playerData.Speed())
				log.Info("Facing: %f", playerData.Facing())
				pos := playerData.Position(nil)
				log.Info("Position: %f, %f, %f", pos.X(), pos.Y(), pos.Z())

				break
			case Nier.PacketTypeID_ANIMATION_START:
				log.Info("Animation start received")

				animationData := &Nier.AnimationStart{}
				flatbuffers.GetRootAs(data.DataBytes(), 0, animationData)

				log.Info("Animation: %d", animationData.Anim())
				log.Info("Variant: %d", animationData.Variant())
				log.Info("a3: %d", animationData.A3())
				log.Info("a4: %d", animationData.A4())

				break
			default:
				log.Error("Unknown packet type: %d", data.Id())
			}

			// Disconnect the peer if they say "bye"
			if string(packetBytes) == "bye" {
				log.Info("Bye!")
				ev.GetPeer().Disconnect(0)
				continue
			}
		}
	}

	// Destroy the host when we're done with it
	host.Destroy()

	// Uninitialize enet
	enet.Deinitialize()
}