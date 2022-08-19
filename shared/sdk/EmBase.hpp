#pragma once

#include "app/EntityGet.hpp"
#include "lib/HashedString.hpp"
#include "BehaviorAppBase.hpp"

namespace sdk {
class EmBase : public sdk::BehaviorAppBase {
public:
    static constexpr std::string_view class_name() {
        return "class EmBase";
    }

public:
    OBJECT_SCRIPT_FUNCTION(EmBase, addExpDirect, void) // base + 0x574040
    OBJECT_SCRIPT_FUNCTION(EmBase, callEnemy, void) // base + 0x537010
    OBJECT_SCRIPT_FUNCTION(EmBase, callSeM1040Guide1, void) // base + 0x5798e0
    OBJECT_SCRIPT_FUNCTION(EmBase, callSeM1040Guide2, void) // base + 0x579910
    OBJECT_SCRIPT_FUNCTION(EmBase, changeEnemy, void) // base + 0x584760
    OBJECT_SCRIPT_FUNCTION(EmBase, changeNPC, void) // base + 0x584e00
    OBJECT_SCRIPT_FUNCTION(EmBase, clearDispHpBar, void) // base + 0x594b70
    OBJECT_SCRIPT_FUNCTION(EmBase, clearHackingStageLabel, void) // base + 0x480190
    OBJECT_SCRIPT_FUNCTION(EmBase, clearSetFlagNoAttackDie, void) // base + 0x594f90
    OBJECT_SCRIPT_FUNCTION(EmBase, clearUndead, void) // base + 0x595160
    OBJECT_SCRIPT_FUNCTION(EmBase, enableLoockCtrlPL, void) // base + 0x5cfc70
    OBJECT_SCRIPT_FUNCTION(EmBase, endCoverBoar, void) // base + 0x5d0c30
    OBJECT_SCRIPT_FUNCTION(EmBase, endDefChase, void) // base + 0x5d0c70
    OBJECT_SCRIPT_FUNCTION(EmBase, endHackingSpStun, void) // base + 0x5d1470
    OBJECT_SCRIPT_FUNCTION(EmBase, endNotExplode, void) // base + 0x5d16b0
    OBJECT_SCRIPT_FUNCTION(EmBase, endSQParade, void) // base + 0x5d2890
    OBJECT_SCRIPT_FUNCTION(EmBase, endSleep, void) // base + 0x5d2940
    OBJECT_SCRIPT_FUNCTION(EmBase, endTraining, void) // base + 0x5d2da0
    OBJECT_SCRIPT_FUNCTION(EmBase, endTrainingB, void) // base + 0x5d2f60
    OBJECT_SCRIPT_FUNCTION(EmBase, forceSandApeear, void) // base + 0x5d4880
    OBJECT_SCRIPT_FUNCTION(EmBase, goRomeoJuliet, void) // base + 0x5dda50
    OBJECT_SCRIPT_FUNCTION(EmBase, hapClearParam, void) // base + 0x5ddc60
    OBJECT_SCRIPT_FUNCTION(EmBase, hapSaveParam, void) // base + 0x5ddc90
    OBJECT_SCRIPT_FUNCTION(EmBase, offAlwaysSuerArmor, void) // base + 0x5efff0
    OBJECT_SCRIPT_FUNCTION(EmBase, offCheckerEff, void) // base + 0x5f0130
    OBJECT_SCRIPT_FUNCTION(EmBase, offDefChase, void) // base + 0x5f01d0
    OBJECT_SCRIPT_FUNCTION(EmBase, offDefScared, void) // base + 0x5f01e0
    OBJECT_SCRIPT_FUNCTION(EmBase, offDmgScared, void) // base + 0x5f0420
    OBJECT_SCRIPT_FUNCTION(EmBase, offEffDojoField, void) // base + 0x5f0450
    OBJECT_SCRIPT_FUNCTION(EmBase, offEm001dStopEnable, void) // base + 0x5f0590
    OBJECT_SCRIPT_FUNCTION(EmBase, offHackingEnable, void) // base + 0x5f06e0
    OBJECT_SCRIPT_FUNCTION(EmBase, offNoDieSuperArmor, void) // base + 0x5f0790
    OBJECT_SCRIPT_FUNCTION(EmBase, onAlwaysSuerArmor, void) // base + 0x5f0d20
    OBJECT_SCRIPT_FUNCTION(EmBase, onBreakArmEnable, void) // base + 0x5f0dc0
    OBJECT_SCRIPT_FUNCTION(EmBase, onDefChase, void) // base + 0x5f0f50
    OBJECT_SCRIPT_FUNCTION(EmBase, onDefScared, void) // base + 0x5f0f60
    OBJECT_SCRIPT_FUNCTION(EmBase, onDmgScared, void) // base + 0x5f1410
    OBJECT_SCRIPT_FUNCTION(EmBase, onEffDojoField, void) // base + 0x5f1550
    OBJECT_SCRIPT_FUNCTION(EmBase, onEm001dStopAction, void) // base + 0x5f18c0
    OBJECT_SCRIPT_FUNCTION(EmBase, onEm001dStopEnable, void) // base + 0x5f18d0
    OBJECT_SCRIPT_FUNCTION(EmBase, onHackingEnable, void) // base + 0x5f1a40
    OBJECT_SCRIPT_FUNCTION(EmBase, onHackingNoReaction, void) // base + 0x5f1a50
    OBJECT_SCRIPT_FUNCTION(EmBase, onNoDieSuperArmor, void) // base + 0x5f1aa0
    OBJECT_SCRIPT_FUNCTION(EmBase, resetEm001dStopTimer, void) // base + 0x6068b0
    OBJECT_SCRIPT_FUNCTION(EmBase, resetSlide, void) // base + 0x607360
    OBJECT_SCRIPT_FUNCTION(EmBase, setBrandHap, void) // base + 0x60eaa0
    OBJECT_SCRIPT_FUNCTION(EmBase, setCoverBoar, void) // base + 0x6134b0
    OBJECT_SCRIPT_FUNCTION(EmBase, setDefChaseType, void, float) // base + 0x613b70
    OBJECT_SCRIPT_FUNCTION(EmBase, setDefenseMoveRtn, void, float) // base + 0x6143b0
    OBJECT_SCRIPT_FUNCTION(EmBase, setDelayedVirus, void) // base + 0x6145c0
    OBJECT_SCRIPT_FUNCTION(EmBase, setDieRtn, void) // base + 0x537028
    OBJECT_SCRIPT_FUNCTION(EmBase, setDieRtnNoDrop, void) // base + 0x6146a0
    OBJECT_SCRIPT_FUNCTION(EmBase, setDisappearDie, void) // base + 0x6146c0
    OBJECT_SCRIPT_FUNCTION(EmBase, setDownLoop, void) // base + 0x615200
    OBJECT_SCRIPT_FUNCTION(EmBase, setEffMosaic, void, int) // base + 0x615400
    OBJECT_SCRIPT_FUNCTION(EmBase, setEm0030Spin, void) // base + 0x616c00
    OBJECT_SCRIPT_FUNCTION(EmBase, setEm1050DieMove, void) // base + 0x61af20
    OBJECT_SCRIPT_FUNCTION(EmBase, setEmb000PowerUp, void) // base + 0x626ec0
    OBJECT_SCRIPT_FUNCTION(EmBase, setFlagWave, void) // base + 0x629090
    OBJECT_SCRIPT_FUNCTION(EmBase, setForceWalk, void, float) // base + 0x629550
    OBJECT_SCRIPT_FUNCTION(EmBase, setGoPoint, void, Hw::cVec4 const &) // base + 0x6299f0
    OBJECT_SCRIPT_FUNCTION(EmBase, setHackingStageLabelByHap, void, class lib::HashedString<struct sys::StringSystem::Allocator> const &) // base + 0x4eedb0
    OBJECT_SCRIPT_FUNCTION(EmBase, setHapAttackTarget, void, class app::EntityGet::ProxyT<class app::Puid> const &) // base + 0x62e640
    OBJECT_SCRIPT_FUNCTION(EmBase, setHpFromRate, void, float) // base + 0x62f200
    OBJECT_SCRIPT_FUNCTION(EmBase, setMutekiTimer, void, float) // base + 0x633ab0
    OBJECT_SCRIPT_FUNCTION(EmBase, setNoDashTimer, void, float) // base + 0x633ae0
    OBJECT_SCRIPT_FUNCTION(EmBase, setNpcScare, void) // base + 0x633bd0
    OBJECT_SCRIPT_FUNCTION(EmBase, setRoarRtn, void) // base + 0x637930
    OBJECT_SCRIPT_FUNCTION(EmBase, setRoboOil, void) // base + 0x637a90
    OBJECT_SCRIPT_FUNCTION(EmBase, setRtnDmageElectricEscape, void) // base + 0x637d40
    OBJECT_SCRIPT_FUNCTION(EmBase, setSandAppearWaitClear, void) // base + 0x637e40
    OBJECT_SCRIPT_FUNCTION(EmBase, setSandDiveEscape, void) // base + 0x637e70
    OBJECT_SCRIPT_FUNCTION(EmBase, setSlideEndPos, void, Hw::cVec4 const &) // base + 0x63dae0
    OBJECT_SCRIPT_FUNCTION(EmBase, setSuicideDrop, void) // base + 0x63e2d0
    OBJECT_SCRIPT_FUNCTION(EmBase, setSurpriseA, void) // base + 0x63e310
    OBJECT_SCRIPT_FUNCTION(EmBase, setSurpriseB, void) // base + 0x63e340
    OBJECT_SCRIPT_FUNCTION(EmBase, setSurpriseC, void) // base + 0x63e370
    OBJECT_SCRIPT_FUNCTION(EmBase, setTargetIcon, void) // base + 0x53701c
    OBJECT_SCRIPT_FUNCTION(EmBase, startDispHpBar, void) // base + 0x642bc0
    OBJECT_SCRIPT_FUNCTION(EmBase, startDispHpBarNoWarning, void) // base + 0x642bd0
    OBJECT_SCRIPT_FUNCTION(EmBase, startRace, void) // base + 0x643240
    OBJECT_SCRIPT_FUNCTION(EmBase, startSQParade, void) // base + 0x6433d0
    OBJECT_SCRIPT_FUNCTION(EmBase, startSQParade2nd, void) // base + 0x6433c0
    OBJECT_SCRIPT_FUNCTION(EmBase, startSlide, void) // base + 0x643620
    OBJECT_SCRIPT_FUNCTION(EmBase, startTraining, void) // base + 0x643cb0
    OBJECT_SCRIPT_FUNCTION(EmBase, startTrainingB, void) // base + 0x643e50
    OBJECT_SCRIPT_FUNCTION(EmBase, stopRomeoJuliet, void) // base + 0x66cee0
    OBJECT_SCRIPT_FUNCTION(EmBase, unlockEmList, void) // base + 0x672100

private:
    uint8_t detail_embase_data[0x2C390 - sizeof(sdk::BehaviorAppBase)];
};

static_assert(sizeof(EmBase) == 0x2C390, "size of EmBase is not correct");
}