package automatamp

import (
	"github.com/codecat/go-enet"
)

type Connection struct {
	peer   enet.Peer
	client *Client
}
