#pragma once

#include "app/Puid.hpp"
#include "BehaviorAppBase.hpp"
#include "ScriptFunctions.hpp"

namespace sdk {
class NPC : public BehaviorAppBase {
public:
    static constexpr std::string_view class_name() {
        return "class NPC";
    }

public:
    OBJECT_SCRIPT_FUNCTION(NPC, changeSetTypeFollow, void) // base + 0x7008f0
    OBJECT_SCRIPT_FUNCTION(NPC, changeSetTypeIdle, void) // base + 0x7009f0
    OBJECT_SCRIPT_FUNCTION(NPC, dieDevola, void) // base + 0x702850
    OBJECT_SCRIPT_FUNCTION(NPC, endPl1100Battle, void) // base + 0x703790
    OBJECT_SCRIPT_FUNCTION(NPC, endPl2040Photograph, void) // base + 0x7037a0
    OBJECT_SCRIPT_FUNCTION(NPC, getRoute, class app::Puid const) // base + 0x703960
    OBJECT_SCRIPT_FUNCTION(NPC, killPl1000Playable, void) // base + 0x706120
    OBJECT_SCRIPT_FUNCTION(NPC, offPascalBattleMode, void) // base + 0x7066b0
    OBJECT_SCRIPT_FUNCTION(NPC, offPascalNoDie, void) // base + 0x7066d0
    OBJECT_SCRIPT_FUNCTION(NPC, onPascalBattleMode, void) // base + 0x7067d0
    OBJECT_SCRIPT_FUNCTION(NPC, onPascalNoDie, void) // base + 0x706880
    OBJECT_SCRIPT_FUNCTION(NPC, requestRoutePuid, class app::Puid const) // base + 0x703960
    OBJECT_SCRIPT_FUNCTION(NPC, setAutoLook, void, bool) // base + 0x708830
    OBJECT_SCRIPT_FUNCTION(NPC, setBreakPl1100, void) // base + 0x708840
    OBJECT_SCRIPT_FUNCTION(NPC, setDisableHacking, void) // base + 0x708b20
    OBJECT_SCRIPT_FUNCTION(NPC, setEnableHacking, void) // base + 0x708b30
    OBJECT_SCRIPT_FUNCTION(NPC, setPl1000BuddyFromNPC, void) // base + 0x708bd0
    OBJECT_SCRIPT_FUNCTION(NPC, setPl1000DieDown, void) // base + 0x708be0
    OBJECT_SCRIPT_FUNCTION(NPC, setPl1000JetComeOut, void) // base + 0x708bf0
    OBJECT_SCRIPT_FUNCTION(NPC, setPl1000JetComeOutWait, void) // base + 0x708c00
    OBJECT_SCRIPT_FUNCTION(NPC, setPl1000JetComeOutWaitReturn, void) // base + 0x708c20
    OBJECT_SCRIPT_FUNCTION(NPC, setPl1000StandUp, void) // base + 0x708c30
    OBJECT_SCRIPT_FUNCTION(NPC, setPl1000WhiteFlagEnd, void) // base + 0x708c40
    OBJECT_SCRIPT_FUNCTION(NPC, setPl1001ComeOut, void) // base + 0x708c50
    OBJECT_SCRIPT_FUNCTION(NPC, setPl1020Close9s, void) // base + 0x708c60
    OBJECT_SCRIPT_FUNCTION(NPC, setPl1020Down, void) // base + 0x708c70
    OBJECT_SCRIPT_FUNCTION(NPC, setPl1020WakeUp, void) // base + 0x708c80
    OBJECT_SCRIPT_FUNCTION(NPC, setRepairPl1000, void) // base + 0x709480
    OBJECT_SCRIPT_FUNCTION(NPC, startPl1040SleepTalk, void) // base + 0x70a240
    OBJECT_SCRIPT_FUNCTION(NPC, startPl1100Battle, void) // base + 0x70a260
    OBJECT_SCRIPT_FUNCTION(NPC, startPl2040Photograph, void) // base + 0x70a270

private:
    uint8_t detail_npc_data[0x24830 - sizeof(BehaviorAppBase)];
};

static_assert(sizeof(NPC) == 0x24830, "Size of NPC is not correct.");
}