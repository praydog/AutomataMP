package structs

import (
	"time"

	"github.com/codecat/go-enet"
)

type Server struct {
	Host              enet.Host
	Connections       map[enet.Peer]*Connection
	Clients           map[*Connection]*Client
	Entities          EntityList
	ConnectionCount   uint64
	HighestEntityGuid uint32
	Config            map[string]interface{}
	LastHeartbeat     time.Time
}
