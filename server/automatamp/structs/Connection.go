package structs

import (
	"github.com/codecat/go-enet"
)

type Connection struct {
	Peer   enet.Peer
	Client *Client
}
