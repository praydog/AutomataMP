package handlers

import (
	"github.com/codecat/go-enet"
	"github.com/codecat/go-libs/log"
	flatbuffers "github.com/google/flatbuffers/go"
	core "github.com/praydog/AutomataMP/server/automatamp/core"
	nier "github.com/praydog/AutomataMP/server/automatamp/nier"
	structs "github.com/praydog/AutomataMP/server/automatamp/structs"
)

func HandleHello(server *structs.Server, sender enet.Peer, connection *structs.Connection, data *nier.Packet) {
	log.Info("Hello packet received")

	if connection.Client != nil {
		log.Error("Received hello packet from client that already has a client, discarding")
		return
	}

	helloData := nier.GetRootAsHello(data.DataBytes(), 0)

	if helloData.Major() != uint32(nier.VersionMajorValue) {
		log.Error("Invalid major version: %d", helloData.Major())
		sender.DisconnectNow(0)
		return
	}

	if helloData.Minor() > uint32(nier.VersionMinorValue) {
		log.Error("Client is newer than server, disconnecting")
		sender.DisconnectNow(0)
		return
	}

	if helloData.Patch() != uint32(nier.VersionPatchValue) {
		log.Info("Minor version mismatch, this is okay")
	}

	log.Info("Version check passed")

	if server.Config["password"].(string) != "" {
		if string(helloData.Password()) != server.Config["password"].(string) {
			log.Error("Invalid password, client sent: \"%s\"", string(helloData.Password()))
			sender.DisconnectNow(0)
			return
		}
	}

	log.Info("Password check passed")

	if _, ok := nier.EnumNamesModelType[nier.ModelType(helloData.Model())]; !ok {
		log.Error("Invalid model type: %d", helloData.Model())
		sender.DisconnectNow(0)
		return
	}

	log.Info("Model type check passed")

	clientName := core.GetFilteredPlayerName(server, helloData.Name())

	server.ConnectionCount++

	// Create a new client for the peer
	client := &structs.Client{
		Guid:  server.ConnectionCount,
		Name:  clientName,
		Model: helloData.Model(),

		// Allows the first person that connects to be the master client
		// In an ideal world, the server would run all of the simulation logic
		// like movement, physics, enemy AI & movement, but this would be
		// a monumental task because this is a mod, not a game where we have the source code.
		// So we let the master client control the simulation.
		IsMasterClient: len(server.Clients) == 0,
	}

	log.Info("Client name: %s", clientName)
	log.Info("Client GUID: %d", client.Guid)
	log.Info("Client is master client: %t", client.IsMasterClient)
	log.Info("Client model: %s", nier.EnumNamesModelType[nier.ModelType(helloData.Model())])

	// Add the client to the map
	connection.Client = client
	server.Clients[connection] = client

	// Send a welcome packet
	welcomeBytes := core.BuilderSurround(func(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
		nier.WelcomeStart(builder)
		nier.WelcomeAddGuid(builder, client.Guid)
		nier.WelcomeAddIsMasterClient(builder, client.IsMasterClient)
		nier.WelcomeAddHighestEntityGuid(builder, server.HighestEntityGuid)
		return nier.WelcomeEnd(builder)
	})

	log.Info("Sending welcome packet")
	sender.SendBytes(core.MakePacketBytes(nier.PacketTypeID_WELCOME, welcomeBytes), 0, enet.PacketFlagReliable)

	// Send the player creation packet
	createPlayerBytes := core.BuilderSurround(func(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
		playerName := builder.CreateString(client.Name)
		nier.CreatePlayerStart(builder)
		nier.CreatePlayerAddGuid(builder, client.Guid)
		nier.CreatePlayerAddName(builder, playerName)
		nier.CreatePlayerAddModel(builder, client.Model)
		return nier.CreatePlayerEnd(builder)
	})

	log.Info("Sending create player packet to everyone")
	core.BroadcastPacketToAll(server, nier.PacketTypeID_CREATE_PLAYER, createPlayerBytes)

	// Broadcast previously connected clients to the new client
	for _, prevClient := range server.Clients {
		if prevClient == nil || prevClient == client { // Skip the new client
			continue
		}

		createPlayerBytes := core.BuilderSurround(func(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
			playerName := builder.CreateString(prevClient.Name)
			nier.CreatePlayerStart(builder)
			nier.CreatePlayerAddGuid(builder, prevClient.Guid)
			nier.CreatePlayerAddName(builder, playerName)
			nier.CreatePlayerAddModel(builder, prevClient.Model)
			return nier.CreatePlayerEnd(builder)
		})

		log.Info("Sending create player packet for previous client %d to client %d", prevClient.Guid, client.Guid)
		sender.SendBytes(core.MakePacketBytes(nier.PacketTypeID_CREATE_PLAYER, createPlayerBytes), 0, enet.PacketFlagReliable)
	}

	// Broadcast previously spawned entities to the new client
	for _, entity := range server.Entities {
		if entity == nil {
			continue
		}

		spawnData := core.BuilderSurround(func(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
			name := builder.CreateString(string(entity.SpawnInfo.Name()))
			posdata := entity.SpawnInfo.Positional(nil)
			nier.EntitySpawnParamsStart(builder)
			nier.EntitySpawnParamsAddName(builder, name)
			nier.EntitySpawnParamsAddModel(builder, entity.SpawnInfo.Model())
			nier.EntitySpawnParamsAddModel2(builder, entity.SpawnInfo.Model2())
			packetPosData := nier.CreateEntitySpawnPositionalData(
				builder,
				posdata.Forward(nil).X(), posdata.Forward(nil).Y(), posdata.Forward(nil).Z(), posdata.Forward(nil).W(),
				posdata.Up(nil).X(), posdata.Up(nil).Y(), posdata.Up(nil).Z(), posdata.Up(nil).W(),
				posdata.Right(nil).X(), posdata.Right(nil).Y(), posdata.Right(nil).Z(), posdata.Right(nil).W(),
				posdata.W(nil).X(), posdata.W(nil).Y(), posdata.W(nil).Z(), posdata.W(nil).W(),
				posdata.Position(nil).X(), posdata.Position(nil).Y(), posdata.Position(nil).Z(), posdata.Position(nil).W(),
				posdata.Unknown(nil).X(), posdata.Unknown(nil).Y(), posdata.Unknown(nil).Z(), posdata.Unknown(nil).W(),
				posdata.Unknown2(nil).X(), posdata.Unknown2(nil).Y(), posdata.Unknown2(nil).Z(), posdata.Unknown2(nil).W(),
				posdata.Unk(), posdata.Unk2(), posdata.Unk3(), posdata.Unk4(),
				posdata.Unk5(), posdata.Unk6(), posdata.Unk7(), posdata.Unk8(),
			)
			nier.EntitySpawnParamsAddPositional(builder, packetPosData)
			return nier.EntitySpawnParamsEnd(builder)
		})

		spawnPacket := core.MakeEntityPacketBytes(entity.Guid, nier.PacketTypeID_SPAWN_ENTITY, spawnData)

		log.Info("Sending spawn entity packet for entity %d to client %d", entity.Guid, client.Guid)
		sender.SendBytes(spawnPacket, 0, enet.PacketFlagReliable)
	}
}
