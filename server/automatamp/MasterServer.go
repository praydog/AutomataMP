// This is the master server. A simple HTTP server that listens for heartbeats from the servers.
// The clients can send a /servers request to obtain the list of servers in JSON format.
package automatamp

import (
	"fmt"
	"net/http"
	"sync"
	"time"

	gin "github.com/gin-gonic/gin"
)

type ServerData struct {
	Name       string `json:"Name" binding:"required"`
	NumPlayers *int   `json:"NumPlayers" binding:"required"`
}

type ActiveServer struct {
	ip       string
	lastBeat time.Time
	Data     ServerData `json:"Data"`
}

type MasterServer struct {
	mtx           sync.Mutex
	servers       map[string]*ActiveServer // ip -> server
	lastCleanTime time.Time
}

func (server *MasterServer) cleanDeadServers() {
	// so the server doesn't get spammed with requests
	if time.Since(server.lastCleanTime) < time.Second*5 {
		return
	}

	for ip, activeServer := range server.servers {
		if time.Since(activeServer.lastBeat) > time.Second*60 {
			fmt.Printf("Deleting server: %s\n", ip)
			delete(server.servers, ip)
			continue
		}
	}

	server.lastCleanTime = time.Now()
}

func (server *MasterServer) listen() {
	r := gin.Default()
	r.POST("/heartbeat", func(c *gin.Context) {
		var json ServerData
		if err := c.ShouldBindJSON(&json); err != nil {
			c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
			return
		}

		server.mtx.Lock()
		defer server.mtx.Unlock()

		ip := c.RemoteIP()

		// create new entry in map if it doesn't exist
		if _, ok := server.servers[ip]; !ok {
			fmt.Printf("Creating new server: %s\n", ip)
			server.servers[ip] = &ActiveServer{ip: ip}
		}

		// Trim the name to 64 characters
		if len(json.Name) > 64 {
			json.Name = json.Name[:64]
		}

		server.servers[ip].Data.Name = json.Name
		server.servers[ip].Data.NumPlayers = json.NumPlayers
		server.servers[ip].lastBeat = time.Now()
		server.cleanDeadServers()

		c.String(http.StatusOK, "OK")
	})

	r.GET("/servers", func(c *gin.Context) {
		server.mtx.Lock()
		defer server.mtx.Unlock()

		server.cleanDeadServers()

		c.JSON(http.StatusOK, server.servers)
	})

	r.GET("/", func(c *gin.Context) {
		c.Redirect(http.StatusMovedPermanently, "https://github.com/praydog/AutomataMP")
	})

	r.Run(":80")
}

func (server *MasterServer) Run() {
	server.listen()
}

func CreateMasterServer() *MasterServer {
	server := &MasterServer{}
	server.servers = make(map[string]*ActiveServer)
	return server
}
