package structs

import nier "github.com/praydog/AutomataMP/server/automatamp/nier"

type ActiveEntity struct {
	Guid      uint32
	SpawnInfo *nier.EntitySpawnParams
	//lastEntityData *nier.EntityData // to be seen if it needs to be used.
}

type EntityList map[uint32]*ActiveEntity
