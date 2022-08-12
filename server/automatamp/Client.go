package automatamp

import nier "automatamp/automatamp/nier"

type Client struct {
	guid           uint64
	model          uint32
	name           string
	isMasterClient bool
	lastPlayerData *nier.PlayerData
}
