package automatamp

import (
	"bytes"
	"context"
	"encoding/json"
	"net"
	"net/http"
	"os"
	"strconv"
	"time"

	core "github.com/praydog/AutomataMP/server/automatamp/core"
	handlers "github.com/praydog/AutomataMP/server/automatamp/handlers"
	nier "github.com/praydog/AutomataMP/server/automatamp/nier"
	structs "github.com/praydog/AutomataMP/server/automatamp/structs"

	"github.com/codecat/go-enet"
	"github.com/codecat/go-libs/log"
)

var currentServer *structs.Server

func handleEnetEvent(ev enet.Event) {
	switch ev.GetType() {
	case enet.EventConnect:
		handleEnetConnectEvent(ev)
	case enet.EventDisconnect:
		handleEnetDisconnectEvent(ev)
	case enet.EventReceive:
		handleEnetReceiveEvent(ev)
	}
}

func handleEnetConnectEvent(ev enet.Event) {
	log.Info("New peer connected: %s", ev.GetPeer().GetAddress())
	connection := &structs.Connection{}
	connection.Peer = ev.GetPeer()
	connection.Client = nil
	currentServer.Connections[ev.GetPeer()] = connection
}

func handleEnetDisconnectEvent(ev enet.Event) {
	log.Info("Peer disconnected: %s", ev.GetPeer().GetAddress())
	connection := currentServer.Connections[ev.GetPeer()]

	if connection != nil {
		handleDisconnect(connection, ev.GetPeer())
	}
}

func handleDisconnect(connection *structs.Connection, peer enet.Peer) {
	isMasterClient := connection.Client != nil && connection.Client.IsMasterClient

	if connection.Client != nil {
		handlers.HandleDestroyPlayer(currentServer, connection)
		delete(currentServer.Clients, connection)
	}

	if isMasterClient {
		handlers.HandleNewMasterClient(currentServer)
	}

	if len(currentServer.Clients) == 0 {
		currentServer.Entities = make(structs.EntityList)
		currentServer.HighestEntityGuid = 0
	}

	delete(currentServer.Connections, peer)
}

func handleEnetReceiveEvent(ev enet.Event) {
	connection := currentServer.Connections[ev.GetPeer()]

	packet := ev.GetPacket()
	defer packet.Destroy()
	packetBytes := packet.GetData()

	if checkClientIsAuthorized(ev, packetBytes) {
		handlers.PacketHandler(currentServer, connection, ev.GetPeer(), packetBytes)
	} else {
		log.Info("Client unauthorized")
	}
}

func checkClientIsAuthorized(ev enet.Event, data []byte) bool {
	connection := currentServer.Connections[ev.GetPeer()]

	if connection == nil {
		log.Error("Received data from unknown peer, ignoring")
		return false
	}

	if connection.Client == nil {
		log.Info("Peer %s sent data %d bytes", ev.GetPeer().GetAddress().String(), len(data))
		data := nier.GetRootAsPacket(data, 0)

		if !core.CheckValidPacket(data) {
			return false
		}

		if data.Id() != nier.PacketTypeID_HELLO {
			log.Error("Received packet before hello was sent, discarding")
			return false
		}
	}

	return true
}

func service() {
	// Wait until the next event
	ev := currentServer.Host.Service(0)
	evHandleCount := 0

	for ev.GetType() != enet.EventNone {
		evHandleCount++
		handleEnetEvent(ev)

		// Break out if we have handled too many events
		// so we can perform other tasks. We will handle the rest
		// next time we call Service.
		if evHandleCount >= 100 {
			log.Info("Handled 100 events, breaking")
			break
		}

		ev = currentServer.Host.Service(0)
	}
}

func cleanup() {
	// Destroy the host when we're done with it
	if currentServer.Host != nil {
		currentServer.Host.Destroy()
	}

	// Uninitialize enet
	enet.Deinitialize()
}

func sendHeartbeatToMasterServer() {
	if !currentServer.Config["masterServerNotify"].(bool) {
		log.Info("Not sending heartbeats to master server. If you want this then set masterServerNotify to true in server.json")
		return
	}

	log.Info("Sending heartbeat to master server")
	currentServer.LastHeartbeat = time.Now()

	jsonValues := make(map[string]interface{})
	jsonValues["Port"] = currentServer.Config["port"].(string)
	jsonValues["Name"] = currentServer.Config["name"].(string)
	jsonValues["NumPlayers"] = len(currentServer.Clients)

	jsonBytes, err := json.Marshal(jsonValues)

	if err != nil {
		log.Error("Error marshalling heartbeat json: %s", err)
		return
	}

	log.Info(string(jsonBytes))

	url := currentServer.Config["masterServer"].(string) + "/heartbeat"
	log.Info("Sending heartbeat to %s", url)

	r, err := http.NewRequest("POST", url, bytes.NewBuffer(jsonBytes))

	if err != nil {
		log.Error("Error creating heartbeat request: %s", err)
		return
	}

	var zeroDialer net.Dialer

	transport := http.DefaultTransport.(*http.Transport).Clone()
	transport.DialContext = func(ctx context.Context, network, addr string) (net.Conn, error) {
		return zeroDialer.DialContext(ctx, "tcp4", addr)
	}

	client := &http.Client{}
	client.Transport = transport
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

func heartbeatGoroutine() {
	if !currentServer.Config["masterServerNotify"].(bool) {
		log.Info("Not sending heartbeats to master server. If you want this then set masterServerNotify to true in server.json")
		return
	}

	for {
		// every 30 seconds, send a heartbeat to the master server
		// containing the server name and how many players are connected
		if time.Since(currentServer.LastHeartbeat) >= 30*time.Second {
			sendHeartbeatToMasterServer()
		}

		time.Sleep(time.Second)
	}
}

// server start
func Run() {
	// Initialize enet
	enet.Initialize()

	defer cleanup()

	// Convert port to int
	port, err := strconv.Atoi(currentServer.Config["port"].(string))

	if err != nil {
		log.Info("Error reading port: %s, using default (6969).", err)
		port = 6969
	}

	// Create a host listening on 0.0.0.0:6969
	host, err := enet.NewHost(enet.NewListenAddress(uint16(port)), 32, 1, 0, 0)
	if err != nil {
		log.Error("Couldn't create host: %s", err.Error())
		panic(err)
	}

	currentServer.Host = host

	log.Info("Created host")

	go heartbeatGoroutine()

	// The event loop
	for {
		service()
	}
}

func CreateServer() {
	serverJson, err := os.ReadFile("server.json")
	if err != nil {
		log.Error("Server requires a server.json file to be present")
		panic(err)
	}

	if currentServer == nil {
		currentServer = &structs.Server{}
	}

	currentServer.Connections = make(map[enet.Peer]*structs.Connection)
	currentServer.Clients = make(map[*structs.Connection]*structs.Client)
	currentServer.Entities = make(structs.EntityList)
	currentServer.Config = make(map[string]interface{})
	currentServer.ConnectionCount = 0
	currentServer.HighestEntityGuid = 0
	currentServer.Config["password"] = ""
	currentServer.Config["masterServer"] = "http://localhost"
	currentServer.Config["masterServerNotify"] = true
	currentServer.Config["name"] = "AutomataMP Server"
	currentServer.Config["port"] = "6969"

	json.Unmarshal(serverJson, &currentServer.Config)
	log.Info("Server password: %s", currentServer.Config["password"].(string))

	Run()
}
