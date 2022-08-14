package automatamp

import (
	"bytes"
	"encoding/json"
	"net/http"
	"os"
	"strconv"
	"time"

	nier "github.com/praydog/AutomataMP/server/automatamp/nier"

	"github.com/codecat/go-enet"
	"github.com/codecat/go-libs/log"
	flatbuffers "github.com/google/flatbuffers/go"
)

type ActiveEntity struct {
	guid      uint32
	spawnInfo *nier.EntitySpawnParams
	//lastEntityData *nier.EntityData // to be seen if it needs to be used.
}

type Server struct {
	host              enet.Host
	connections       map[enet.Peer]*Connection
	clients           map[*Connection]*Client
	entities          map[uint32]*ActiveEntity
	connectionCount   uint64
	highestEntityGuid uint32
	config            map[string]interface{}
	lastHeartbeat     time.Time
}

func (server *Server) BroadcastPacketToAll(id nier.PacketType, data []uint8) {
	broadcastData := makePacketBytes(id, data)

	for conn := range server.clients {
		conn.peer.SendBytes(broadcastData, 0, enet.PacketFlagReliable)
	}
}

func (server *Server) BroadcastPacketToAllExceptSender(sender enet.Peer, id nier.PacketType, data []uint8) {
	broadcastData := makePacketBytes(id, data)

	for conn := range server.clients {
		if conn.peer == sender {
			continue
		}

		conn.peer.SendBytes(broadcastData, 0, enet.PacketFlagReliable)
	}
}

func (server *Server) BroadcastPlayerPacketToAll(connection *Connection, id nier.PacketType, data []uint8) {
	broadcastData := makePlayerPacketBytes(connection, id, data)

	for conn := range server.clients {
		conn.peer.SendBytes(broadcastData, 0, enet.PacketFlagReliable)
	}
}

func (server *Server) BroadcastPlayerPacketToAllExceptSender(sender enet.Peer, connection *Connection, id nier.PacketType, data []uint8) {
	broadcastData := makePlayerPacketBytes(connection, id, data)

	for conn := range server.clients {
		if conn.peer == sender {
			continue
		}

		conn.peer.SendBytes(broadcastData, 0, enet.PacketFlagReliable)
	}
}

func (server *Server) GetFilteredPlayerName(input []uint8) string {
	out := ""

	if input == nil || string(input) == "" {
		log.Error("Client sent empty name, assigning random name")
		out = "Client" + strconv.FormatInt(int64(len(server.connections)), 10)
	} else {
		out = string(input)
	}

	for _, client := range server.clients {
		if client.name == out {
			log.Error("Client sent duplicate name, appending number")
			out = out + strconv.FormatInt(int64(len(server.connections)), 10)
			break
		}
	}

	return out
}

func (server *Server) handleHello(ev enet.Event, connection *Connection, data *nier.Packet) {
	log.Info("Hello packet received")

	if connection.client != nil {
		log.Error("Received hello packet from client that already has a client, discarding")
		return
	}

	helloData := nier.GetRootAsHello(data.DataBytes(), 0)

	if helloData.Major() != uint32(nier.VersionMajorValue) {
		log.Error("Invalid major version: %d", helloData.Major())
		ev.GetPeer().DisconnectNow(0)
		return
	}

	if helloData.Minor() > uint32(nier.VersionMinorValue) {
		log.Error("Client is newer than server, disconnecting")
		ev.GetPeer().DisconnectNow(0)
		return
	}

	if helloData.Patch() != uint32(nier.VersionPatchValue) {
		log.Info("Minor version mismatch, this is okay")
	}

	log.Info("Version check passed")

	if server.config["password"].(string) != "" {
		if string(helloData.Password()) != server.config["password"].(string) {
			log.Error("Invalid password, client sent: \"%s\"", string(helloData.Password()))
			ev.GetPeer().DisconnectNow(0)
			return
		}
	}

	log.Info("Password check passed")

	if _, ok := nier.EnumNamesModelType[nier.ModelType(helloData.Model())]; !ok {
		log.Error("Invalid model type: %d", helloData.Model())
		ev.GetPeer().DisconnectNow(0)
		return
	}

	log.Info("Model type check passed")

	clientName := server.GetFilteredPlayerName(helloData.Name())

	server.connectionCount++

	// Create a new client for the peer
	client := &Client{
		guid:  server.connectionCount,
		name:  clientName,
		model: helloData.Model(),

		// Allows the first person that connects to be the master client
		// In an ideal world, the server would run all of the simulation logic
		// like movement, physics, enemy AI & movement, but this would be
		// a monumental task because this is a mod, not a game where we have the source code.
		// So we let the master client control the simulation.
		isMasterClient: len(server.clients) == 0,
	}

	log.Info("Client name: %s", clientName)
	log.Info("Client GUID: %d", client.guid)
	log.Info("Client is master client: %t", client.isMasterClient)

	// Add the client to the map
	connection.client = client
	server.clients[connection] = client

	// Send a welcome packet
	welcomeBytes := builderSurround(func(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
		nier.WelcomeStart(builder)
		nier.WelcomeAddGuid(builder, client.guid)
		nier.WelcomeAddIsMasterClient(builder, client.isMasterClient)
		nier.WelcomeAddHighestEntityGuid(builder, server.highestEntityGuid)
		return nier.WelcomeEnd(builder)
	})

	log.Info("Sending welcome packet")
	ev.GetPeer().SendBytes(makePacketBytes(nier.PacketTypeID_WELCOME, welcomeBytes), 0, enet.PacketFlagReliable)

	// Send the player creation packet
	createPlayerBytes := builderSurround(func(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
		playerName := builder.CreateString(client.name)
		nier.CreatePlayerStart(builder)
		nier.CreatePlayerAddGuid(builder, client.guid)
		nier.CreatePlayerAddName(builder, playerName)
		nier.CreatePlayerAddModel(builder, client.model)
		return nier.CreatePlayerEnd(builder)
	})

	log.Info("Sending create player packet to everyone")
	server.BroadcastPacketToAll(nier.PacketTypeID_CREATE_PLAYER, createPlayerBytes)

	// Broadcast previously connected clients to the new client
	for _, prevClient := range server.clients {
		if prevClient == nil || prevClient == client { // Skip the new client
			continue
		}

		createPlayerBytes := builderSurround(func(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
			playerName := builder.CreateString(prevClient.name)
			nier.CreatePlayerStart(builder)
			nier.CreatePlayerAddGuid(builder, prevClient.guid)
			nier.CreatePlayerAddName(builder, playerName)
			nier.CreatePlayerAddModel(builder, prevClient.model)
			return nier.CreatePlayerEnd(builder)
		})

		log.Info("Sending create player packet for previous client %d to client %d", prevClient.guid, client.guid)
		ev.GetPeer().SendBytes(makePacketBytes(nier.PacketTypeID_CREATE_PLAYER, createPlayerBytes), 0, enet.PacketFlagReliable)
	}

	// Broadcast previously spawned entities to the new client
	for _, entity := range server.entities {
		if entity == nil {
			continue
		}

		spawnData := builderSurround(func(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
			name := builder.CreateString(string(entity.spawnInfo.Name()))
			posdata := entity.spawnInfo.Positional(nil)
			nier.EntitySpawnParamsStart(builder)
			nier.EntitySpawnParamsAddName(builder, name)
			nier.EntitySpawnParamsAddModel(builder, entity.spawnInfo.Model())
			nier.EntitySpawnParamsAddModel2(builder, entity.spawnInfo.Model2())
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

		spawnPacket := makeEntityPacketBytes(entity.guid, nier.PacketTypeID_SPAWN_ENTITY, spawnData)

		log.Info("Sending spawn entity packet for entity %d to client %d", entity.guid, client.guid)
		ev.GetPeer().SendBytes(spawnPacket, 0, enet.PacketFlagReliable)
	}
}

func (server *Server) handlePlayerData(ev enet.Event, connection *Connection, data *nier.Packet) {
	playerData := &nier.PlayerData{}
	flatbuffers.GetRootAs(data.DataBytes(), 0, playerData)

	connection.client.lastPlayerData = playerData

	// Broadcast the packet back to all valid clients (except the sender)
	server.BroadcastPlayerPacketToAllExceptSender(ev.GetPeer(), connection, nier.PacketTypeID_PLAYER_DATA, data.DataBytes())
}

func (server *Server) handleAnimationStart(ev enet.Event, connection *Connection, data *nier.Packet) {
	log.Info("Animation start received")

	animationData := &nier.AnimationStart{}
	flatbuffers.GetRootAs(data.DataBytes(), 0, animationData)

	log.Info(" Animation: %d", animationData.Anim())
	log.Info(" Variant: %d", animationData.Variant())
	log.Info(" a3: %d", animationData.A3())
	log.Info(" a4: %d", animationData.A4())

	// TODO: sanitize the data

	// Broadcast the packet back to all valid clients (except the sender)
	server.BroadcastPlayerPacketToAllExceptSender(ev.GetPeer(), connection, nier.PacketTypeID_ANIMATION_START, data.DataBytes())
}

func (server *Server) handleButtons(ev enet.Event, connection *Connection, data *nier.Packet) {
	log.Info("Buttons received")

	// Broadcast the packet back to all valid clients (except the sender)
	server.BroadcastPlayerPacketToAllExceptSender(ev.GetPeer(), connection, nier.PacketTypeID_BUTTONS, data.DataBytes())
}

func (server *Server) handleSpawnEntity(ev enet.Event, connection *Connection, data *nier.Packet) {
	log.Info("Spawn entity received")
	if !connection.client.isMasterClient {
		log.Info(" Not a master client, ignoring")
		return
	}

	// Cache the entity.
	entityPkt := &nier.EntityPacket{}
	flatbuffers.GetRootAs(data.DataBytes(), 0, entityPkt)

	spawnInfo := &nier.EntitySpawnParams{}
	flatbuffers.GetRootAs(entityPkt.DataBytes(), 0, spawnInfo)

	if entityPkt.Guid() > server.highestEntityGuid {
		server.highestEntityGuid = entityPkt.Guid()
	}

	server.entities[entityPkt.Guid()] = new(ActiveEntity)
	server.entities[entityPkt.Guid()].guid = entityPkt.Guid()
	server.entities[entityPkt.Guid()].spawnInfo = spawnInfo

	server.BroadcastPacketToAllExceptSender(ev.GetPeer(), nier.PacketTypeID_SPAWN_ENTITY, data.DataBytes())
}

func (server *Server) handleDestroyEntity(ev enet.Event, connection *Connection, data *nier.Packet) {
	log.Info("Destroy entity received")
	if !connection.client.isMasterClient {
		log.Info(" Not a master client, ignoring")
		return
	}

	// Destroy the entity.
	entityPkt := &nier.EntityPacket{}
	flatbuffers.GetRootAs(data.DataBytes(), 0, entityPkt)

	delete(server.entities, entityPkt.Guid())

	server.BroadcastPacketToAllExceptSender(ev.GetPeer(), nier.PacketTypeID_DESTROY_ENTITY, data.DataBytes())
}

func (server *Server) handleEntityData(ev enet.Event, connection *Connection, data *nier.Packet) {
	//log.Info("Entity data received")
	if !connection.client.isMasterClient {
		log.Info(" Not a master client, ignoring")
		return
	}

	server.BroadcastPacketToAllExceptSender(ev.GetPeer(), nier.PacketTypeID_ENTITY_DATA, data.DataBytes())
}

func (server *Server) handleEntityAnimationStart(ev enet.Event, connection *Connection, data *nier.Packet) {
	log.Info("ENTITY Animation start received")

	if !connection.client.isMasterClient {
		log.Info(" Not a master client, ignoring")
		return
	}

	entityPkt := &nier.EntityPacket{}
	flatbuffers.GetRootAs(data.DataBytes(), 0, entityPkt)

	animationData := &nier.AnimationStart{}
	flatbuffers.GetRootAs(entityPkt.DataBytes(), 0, animationData)

	log.Info(" Animation: %d", animationData.Anim())
	log.Info(" Variant: %d", animationData.Variant())
	log.Info(" a3: %d", animationData.A3())
	log.Info(" a4: %d", animationData.A4())

	// TODO: sanitize the data

	// Broadcast the packet back to all valid clients (except the sender)
	server.BroadcastPacketToAllExceptSender(ev.GetPeer(), nier.PacketTypeID_ENTITY_ANIMATION_START, data.DataBytes())
}

func (server *Server) handleEvent(ev enet.Event) {
	switch ev.GetType() {
	case enet.EventConnect: // A new peer has connected
		log.Info("New peer connected: %s", ev.GetPeer().GetAddress())
		connection := &Connection{}
		connection.peer = ev.GetPeer()
		connection.client = nil
		server.connections[ev.GetPeer()] = connection
		break

	case enet.EventDisconnect: // A connected peer has disconnected
		log.Info("Peer disconnected: %s", ev.GetPeer().GetAddress())
		if server.connections[ev.GetPeer()] != nil {
			isMasterClient := server.connections[ev.GetPeer()].client != nil && server.connections[ev.GetPeer()].client.isMasterClient

			// Broadcast a destroy player packet to everyone except the disconnected peer
			if server.connections[ev.GetPeer()].client != nil {
				destroyPlayerBytes := builderSurround(func(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
					return nier.CreateDestroyPlayer(builder, server.connections[ev.GetPeer()].client.guid)
				})

				server.BroadcastPacketToAllExceptSender(ev.GetPeer(), nier.PacketTypeID_DESTROY_PLAYER, destroyPlayerBytes)
			}

			delete(server.clients, server.connections[ev.GetPeer()])

			if isMasterClient {
				// we must find a new master client
				for conn, client := range server.clients {
					log.Info("Setting new master client: %s @ %s", client.name, conn.peer.GetAddress())

					client.isMasterClient = true
					conn.peer.SendBytes(makeEmptyPacketBytes(nier.PacketTypeID_SET_MASTER_CLIENT), 0, enet.PacketFlagReliable)
					break
				}
			}
		}

		delete(server.connections, ev.GetPeer())
		break

	case enet.EventReceive: // A peer sent us some data
		connection := server.connections[ev.GetPeer()]

		if connection == nil {
			log.Error("Received data from unknown peer, ignoring")
			break
		}

		// Get the packet
		packet := ev.GetPacket()

		// We must destroy the packet when we're done with it
		defer packet.Destroy()

		// Get the bytes in the packet
		packetBytes := packet.GetData()

		if connection.client != nil {
			//log.Info("Peer %d @ %s sent data %d bytes", connection.client.guid, ev.GetPeer().GetAddress().String(), len(packetBytes))
		} else {
			log.Info("Peer %s sent data %d bytes", ev.GetPeer().GetAddress().String(), len(packetBytes))
		}

		data := nier.GetRootAsPacket(packetBytes, 0)

		if !checkValidPacket(data) {
			break
		}

		if data.Id() != nier.PacketTypeID_HELLO && connection.client == nil {
			log.Error("Received packet before hello was sent, discarding")
			break
		}

		switch data.Id() {
		case nier.PacketTypeID_HELLO:
			server.handleHello(ev, connection, data)
			break
		case nier.PacketTypeID_PING:
			log.Info("Ping received from %s", connection.client.name)

			ev.GetPeer().SendBytes(makeEmptyPacketBytes(nier.PacketTypeID_PONG), 0, enet.PacketFlagReliable)
			break
		case nier.PacketTypeID_PLAYER_DATA:
			server.handlePlayerData(ev, connection, data)
			break
		case nier.PacketTypeID_ANIMATION_START:
			server.handleAnimationStart(ev, connection, data)
			break
		case nier.PacketTypeID_BUTTONS:
			server.handleButtons(ev, connection, data)
			break
		case nier.PacketTypeID_SPAWN_ENTITY:
			server.handleSpawnEntity(ev, connection, data)
			break
		case nier.PacketTypeID_DESTROY_ENTITY:
			server.handleDestroyEntity(ev, connection, data)
			break
		case nier.PacketTypeID_ENTITY_DATA:
			server.handleEntityData(ev, connection, data)
			break
		case nier.PacketTypeID_ENTITY_ANIMATION_START:
			server.handleEntityAnimationStart(ev, connection, data)
			break
		default:
			log.Error("Unknown packet type: %d", data.Id())
			break
		}
	}
}

func (server *Server) service() {
	// Wait until the next event
	ev := server.host.Service(0)
	evHandleCount := 0

	for ev.GetType() != enet.EventNone {
		evHandleCount++
		server.handleEvent(ev)

		// Break out if we have handled too many events
		// so we can perform other tasks. We will handle the rest
		// next time we call Service.
		if evHandleCount >= 100 {
			log.Info("Handled 100 events, breaking")
			break
		}

		ev = server.host.Service(0)
	}
}

func (server *Server) cleanup() {
	// Destroy the host when we're done with it
	if server.host != nil {
		server.host.Destroy()
	}

	// Uninitialize enet
	enet.Deinitialize()
}

func (server *Server) sendHeartbeatToMasterServer() {
	log.Info("Sending heartbeat to master server")
	server.lastHeartbeat = time.Now()

	jsonValues := make(map[string]interface{})
	jsonValues["Name"] = server.config["name"].(string)
	jsonValues["NumPlayers"] = len(server.clients)

	jsonBytes, err := json.Marshal(jsonValues)

	if err != nil {
		log.Error("Error marshalling heartbeat json: %s", err)
		return
	}

	log.Info(string(jsonBytes))

	url := server.config["masterServer"].(string) + "/heartbeat"
	log.Info("Sending heartbeat to %s", url)

	r, err := http.NewRequest("POST", url, bytes.NewBuffer(jsonBytes))

	if err != nil {
		log.Error("Error creating heartbeat request: %s", err)
		return
	}

	client := &http.Client{}
	resp, err := client.Do(r)

	if err != nil {
		log.Error("Error creating heartbeat request: %s", err)
		return
	}

	if resp.StatusCode != http.StatusOK {
		log.Error("Error sending heartbeat: %s", resp.Status)
		return
	}
}

func (server *Server) heartbeatGoroutine() {
	for {
		// every 30 seconds, send a heartbeat to the master server
		// containing the server name and how many players are connected
		if time.Since(server.lastHeartbeat) >= 30*time.Second {
			server.sendHeartbeatToMasterServer()
		}
	}
}

// server start
func (server *Server) Run() {
	// Initialize enet
	enet.Initialize()

	defer server.cleanup()

	// Create a host listening on 0.0.0.0:6969
	host, err := enet.NewHost(enet.NewListenAddress(6969), 32, 1, 0, 0)
	if err != nil {
		log.Error("Couldn't create host: %s", err.Error())
		panic(err)
	}

	server.host = host

	log.Info("Created host")

	go server.heartbeatGoroutine()

	// The event loop
	for {
		server.service()
	}
}

func CreateServer() *Server {
	serverJson, err := os.ReadFile("server.json")
	if err != nil {
		log.Error("Server requires a server.json file to be present")
		panic(err)
	}

	server := &Server{}
	server.connections = make(map[enet.Peer]*Connection)
	server.clients = make(map[*Connection]*Client)
	server.entities = make(map[uint32]*ActiveEntity)
	server.config = make(map[string]interface{})
	server.connectionCount = 0
	server.highestEntityGuid = 0
	server.config["password"] = ""
	server.config["masterServer"] = "https://niermaster.praydog.com"
	server.config["name"] = "AutomataMP Server"

	json.Unmarshal(serverJson, &server.config)
	log.Info("Server password: %s", server.config["password"].(string))

	return server
}
