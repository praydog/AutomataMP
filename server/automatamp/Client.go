package automatamp

import nier "github.com/praydog/AutomataMP/server/automatamp/nier"

type Client struct {
	guid           uint64
	model          uint32
	name           string
	isMasterClient bool
	lastPlayerData *nier.PlayerData
}
