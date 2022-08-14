package automatamp

import (
	"fmt"
	"net/http"
	"sync"
	"time"

	gin "github.com/gin-gonic/gin"
)

type ServerData struct {
	Name       string `json:"name" binding:"required"`
	NumPlayers int    `json:"numPlayers" binding:"required"`
}

type ActiveServer struct {
	ip       string
	lastBeat int64
	Data     ServerData `json:"data"`
}

type MasterServer struct {
	mtx           sync.Mutex
	servers       map[string]*ActiveServer // ip -> server
	lastCleanTime int64
}

func (server *MasterServer) cleanDeadServers() {
	// so the server doesn't get spammed with requests
	if time.Now().Unix()-server.lastCleanTime < 5 {
		return
	}

	for ip, activeServer := range server.servers {
		if time.Now().Unix()-activeServer.lastBeat > 5 {
			fmt.Printf("Deleting server: %s", ip)
			delete(server.servers, ip)
			continue
		}
	}

	server.lastCleanTime = time.Now().Unix()
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
			fmt.Printf("Creating new server: %s", ip)
			server.servers[ip] = &ActiveServer{ip: ip}
		}

		server.servers[ip].Data.Name = json.Name
		server.servers[ip].Data.NumPlayers = json.NumPlayers
		server.servers[ip].lastBeat = time.Now().Unix()
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
		c.String(http.StatusOK, "Welcome to the NieR: Automata MP Master Server!")
	})

	r.Run()
}

func (server *MasterServer) Run() {
	server.listen()
}

func CreateMasterServer() *MasterServer {
	server := &MasterServer{}
	server.servers = make(map[string]*ActiveServer)
	return server
}
