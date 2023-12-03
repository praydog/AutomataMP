package structs

import nier "github.com/praydog/AutomataMP/server/automatamp/nier"

type Client struct {
	Guid           uint64
	Model          uint32
	Name           string
	IsMasterClient bool
	LastPlayerData *nier.PlayerData
}
