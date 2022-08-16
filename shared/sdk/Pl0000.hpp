#pragma once

#include <utility/RTTI.hpp>

#include "Math.hpp"
#include "regenny/Pl0000.hpp"
#include "BehaviorAppBase.hpp"

#include "ScriptFunctions.hpp"

namespace app {
class Puid;
}

namespace sdk {
class Pl0000 : public sdk::BehaviorAppBase {
public:
    enum EButtonIndex {
        INDEX_ATTACK_LIGHT = 0,
        INDEX_ATTACK_HEAVY = 1,
        INDEX_JUMP = 2,
        INDEX_POD_SPECIAL = 4,
        INDEX_POD_FIRE = 6,
        INDEX_DASH = 7,
        INDEX_MAX = INDEX_DASH + 1,
    };

    __forceinline ::regenny::Pl0000* regenny() const {
        return (::regenny::Pl0000*)this;
    }

    __forceinline uint32_t& buddy_handle() {
        return regenny()->buddy_handle;
    }

    __forceinline regenny::CharacterController& character_controller() {
        return regenny()->controller;
    }

    __forceinline float& facing2() {
        return character_controller().facing;
    }

    __forceinline uint32_t& weapon_index() {
        return regenny()->weapon_index;
    }

    __forceinline uint32_t& pod_index() {
        return regenny()->pod_index;
    }

    __forceinline bool& flashlight() {
        return regenny()->flashlight;
    }

    __forceinline auto& run_speed_type() {
        return regenny()->run_speed_type;
    }

    __forceinline float& speed() {
        return character_controller().speed;
    }

public:
    OBJECT_SCRIPT_FUNCTION(Pl0000, addRedGirl, void) // base + 0x471bb0
    OBJECT_SCRIPT_FUNCTION(Pl0000, callDialogTutorial, void, int) // base + 0x474700
    OBJECT_SCRIPT_FUNCTION(Pl0000, changeFlightUnitBlackToWhite, void) // base + 0x4761d0
    OBJECT_SCRIPT_FUNCTION(Pl0000, changePlayer, void) // base + 0x476220
    OBJECT_SCRIPT_FUNCTION(Pl0000, changePlayerFinal, void) // base + 0x476350
    OBJECT_SCRIPT_FUNCTION(Pl0000, changePlayerFinalA2, void) // base + 0x476430
    OBJECT_SCRIPT_FUNCTION(Pl0000, clear2BDieFilter, void) // base + 0x47fae0
    OBJECT_SCRIPT_FUNCTION(Pl0000, clearBrand, void) // base + 0x47fc30
    OBJECT_SCRIPT_FUNCTION(Pl0000, clearControlHacked, void) // base + 0x47fcd0
    OBJECT_SCRIPT_FUNCTION(Pl0000, clearForceEmpNoise, void) // base + 0x47fe00
    OBJECT_SCRIPT_FUNCTION(Pl0000, clearGetOnFlightUnit, void) // base + 0x4800f0
    OBJECT_SCRIPT_FUNCTION(Pl0000, clearHackingStageLabel, void) // base + 0x480190
    OBJECT_SCRIPT_FUNCTION(Pl0000, clearLimitAction, void) // base + 0x4802c0
    OBJECT_SCRIPT_FUNCTION(Pl0000, clearRunTarget, void) // base + 0x4806a0
    OBJECT_SCRIPT_FUNCTION(Pl0000, clearShootingPosResetTimer, void) // base + 0x480720
    OBJECT_SCRIPT_FUNCTION(Pl0000, clearShootingSideOver, void) // base + 0x480730
    OBJECT_SCRIPT_FUNCTION(Pl0000, clearUINoiseFilter, void) // base + 0x480e80
    OBJECT_SCRIPT_FUNCTION(Pl0000, clearUIPollutionGauge, void) // base + 0x480eb0
    OBJECT_SCRIPT_FUNCTION(Pl0000, clearWalkTarget, void) // base + 0x4806a0
    OBJECT_SCRIPT_FUNCTION(Pl0000, destroyBuddy, void) // base + 0x486290
    OBJECT_SCRIPT_FUNCTION(Pl0000, disableFriendlyFire, void) // base + 0x486480
    OBJECT_SCRIPT_FUNCTION(Pl0000, disableFriendlyFireYorha, void) // base + 0x486490
    OBJECT_SCRIPT_FUNCTION(Pl0000, dispWeaponIn, void) // base + 0x4867e0
    OBJECT_SCRIPT_FUNCTION(Pl0000, dispWeaponInNoEff, void) // base + 0x486950
    OBJECT_SCRIPT_FUNCTION(Pl0000, dispWeaponInReserve, void) // base + 0x4869f0
    OBJECT_SCRIPT_FUNCTION(Pl0000, dispWeaponOut, void) // base + 0x486a00
    OBJECT_SCRIPT_FUNCTION(Pl0000, dispWeaponOutNoEff, void) // base + 0x486ab0
    OBJECT_SCRIPT_FUNCTION(Pl0000, enableFriendlyFire, void) // base + 0x486d60
    OBJECT_SCRIPT_FUNCTION(Pl0000, enableFriendlyFireYorha, void) // base + 0x486d70
    OBJECT_SCRIPT_FUNCTION(Pl0000, enableHackingFriendlyFire, void) // base + 0x486d80
    OBJECT_SCRIPT_FUNCTION(Pl0000, end2BBreak, void) // base + 0x486e00
    OBJECT_SCRIPT_FUNCTION(Pl0000, end9sBreak, void) // base + 0x486e20
    OBJECT_SCRIPT_FUNCTION(Pl0000, endA2Break, void) // base + 0x486ee0
    OBJECT_SCRIPT_FUNCTION(Pl0000, endBehindFreeShooting, void) // base + 0x488150
    OBJECT_SCRIPT_FUNCTION(Pl0000, endControlHack, void, int) // base + 0x4881c0
    OBJECT_SCRIPT_FUNCTION(Pl0000, endControlHackNoBomb, void) // base + 0x4885f0
    OBJECT_SCRIPT_FUNCTION(Pl0000, endEm1100FinalFlightBattle, void) // base + 0x488800
    OBJECT_SCRIPT_FUNCTION(Pl0000, endEm1100FlightBattle, void) // base + 0x488810
    OBJECT_SCRIPT_FUNCTION(Pl0000, endEm4000FlightBattle, void) // base + 0x488820
    OBJECT_SCRIPT_FUNCTION(Pl0000, endEventConnect, void) // base + 0x488830
    OBJECT_SCRIPT_FUNCTION(Pl0000, endEventWeapon, void) // base + 0x488880
    OBJECT_SCRIPT_FUNCTION(Pl0000, endForceAttackEnable, void) // base + 0x488c60
    OBJECT_SCRIPT_FUNCTION(Pl0000, endHackingLocation, void) // base + 0x488eb0
    OBJECT_SCRIPT_FUNCTION(Pl0000, endNoDmgSubQuest, void) // base + 0x4890c0
    OBJECT_SCRIPT_FUNCTION(Pl0000, endPhaseEventFlightUnit, void) // base + 0x4890d0
    OBJECT_SCRIPT_FUNCTION(Pl0000, endPodMoveMode, void) // base + 0x4890e0
    OBJECT_SCRIPT_FUNCTION(Pl0000, endShootingP100Robot, void) // base + 0x488820
    OBJECT_SCRIPT_FUNCTION(Pl0000, endTraining, void) // base + 0x489b10
    OBJECT_SCRIPT_FUNCTION(Pl0000, endVirusMode, void) // base + 0x489b20
    OBJECT_SCRIPT_FUNCTION(Pl0000, endWeakMode, void) // base + 0x489b30
    OBJECT_SCRIPT_FUNCTION(Pl0000, enemyDestroyed, void) // base + 0x489b40
    OBJECT_SCRIPT_FUNCTION(Pl0000, enemyDestroyedLocation, void, const Vector4f&) // base + 0x489d60
    OBJECT_SCRIPT_FUNCTION(Pl0000, hasBuddy, bool) // base + 0x495a10
    OBJECT_SCRIPT_FUNCTION(Pl0000, initBehindFreeShooting, void) // base + 0x497ff0
    OBJECT_SCRIPT_FUNCTION(Pl0000, initEm1100FinalFlightBattle, void) // base + 0x4985d0
    OBJECT_SCRIPT_FUNCTION(Pl0000, initEm1100FlightBattle, void) // base + 0x4985f0
    OBJECT_SCRIPT_FUNCTION(Pl0000, initEm4000FlightBattle, void) // base + 0x498610
    OBJECT_SCRIPT_FUNCTION(Pl0000, isActiveSkillSonar, bool) // base + 0x49d270
    OBJECT_SCRIPT_FUNCTION(Pl0000, isRoboOilFull, bool) // base + 0x49ea80
    OBJECT_SCRIPT_FUNCTION(Pl0000, lostRedGirl, void) // base + 0x4a04e0
    OBJECT_SCRIPT_FUNCTION(Pl0000, offBattleWait, void) // base + 0x4a2080
    OBJECT_SCRIPT_FUNCTION(Pl0000, offCameraShakeEff, void) // base + 0x4a2090
    OBJECT_SCRIPT_FUNCTION(Pl0000, offEm3000TopViewCamera, void) // base + 0x4a2120
    OBJECT_SCRIPT_FUNCTION(Pl0000, offEmileBullet, void) // base + 0x4a2130
    OBJECT_SCRIPT_FUNCTION(Pl0000, offFlashLight, void) // base + 0x4a2140
    OBJECT_SCRIPT_FUNCTION(Pl0000, offFlightUnitMissilePod, void) // base + 0x4a21b0
    OBJECT_SCRIPT_FUNCTION(Pl0000, offLookFlower, void) // base + 0x4a2270
    OBJECT_SCRIPT_FUNCTION(Pl0000, offNPCShootingFreePlay, void) // base + 0x4a22b0
    OBJECT_SCRIPT_FUNCTION(Pl0000, offSleepFade, void) // base + 0x4a22c0
    OBJECT_SCRIPT_FUNCTION(Pl0000, offTalkBattleWait, void) // base + 0x4a22d0
    OBJECT_SCRIPT_FUNCTION(Pl0000, offTowerChangeEff, void) // base + 0x4a22e0
    OBJECT_SCRIPT_FUNCTION(Pl0000, offTowerMode, void) // base + 0x4a2350
    OBJECT_SCRIPT_FUNCTION(Pl0000, offTowerShooting, void) // base + 0x4a2360
    OBJECT_SCRIPT_FUNCTION(Pl0000, offVSCamera, void) // base + 0x4a23a0
    OBJECT_SCRIPT_FUNCTION(Pl0000, offVSCameraAlienShip, void) // base + 0x4a23b0
    OBJECT_SCRIPT_FUNCTION(Pl0000, offVSCameraTowerTop, void) // base + 0x4a23c0
    OBJECT_SCRIPT_FUNCTION(Pl0000, offVirusNoRecovery, void) // base + 0x4a23d0
    OBJECT_SCRIPT_FUNCTION(Pl0000, onBattleWait, void) // base + 0x4a2660
    OBJECT_SCRIPT_FUNCTION(Pl0000, onCameraBigShakeEff, void) // base + 0x4a2670
    OBJECT_SCRIPT_FUNCTION(Pl0000, onCameraShakeEff, void) // base + 0x4a2740
    OBJECT_SCRIPT_FUNCTION(Pl0000, onEm3000TopViewCamera, void) // base + 0x4a2bd0
    OBJECT_SCRIPT_FUNCTION(Pl0000, onEmileBullet, void) // base + 0x4a2be0
    OBJECT_SCRIPT_FUNCTION(Pl0000, onFlashLight, void) // base + 0x4a2bf0
    OBJECT_SCRIPT_FUNCTION(Pl0000, onLookFlower, void) // base + 0x4a3060
    OBJECT_SCRIPT_FUNCTION(Pl0000, onNPCShootingFreePlay, void) // base + 0x4a3070
    OBJECT_SCRIPT_FUNCTION(Pl0000, onSleepFade, void) // base + 0x4a30d0
    OBJECT_SCRIPT_FUNCTION(Pl0000, onTalkBattleWait, void) // base + 0x4a30f0
    OBJECT_SCRIPT_FUNCTION(Pl0000, onTowerChangeEff, void) // base + 0x4a3150
    OBJECT_SCRIPT_FUNCTION(Pl0000, onTowerMode, void) // base + 0x4a31e0
    OBJECT_SCRIPT_FUNCTION(Pl0000, onVSCamera, void) // base + 0x4a3200
    OBJECT_SCRIPT_FUNCTION(Pl0000, onVSCameraAlienShip, void) // base + 0x4a3210
    OBJECT_SCRIPT_FUNCTION(Pl0000, onVSCameraTowerTop, void) // base + 0x4a3220
    OBJECT_SCRIPT_FUNCTION(Pl0000, onVirusNoRecovery, void) // base + 0x4a3230
    OBJECT_SCRIPT_FUNCTION(Pl0000, pauseSTG, void, int) // base + 0x355f90
    OBJECT_SCRIPT_FUNCTION(Pl0000, resetNPCShootingPos, void) // base + 0x4e6840
    OBJECT_SCRIPT_FUNCTION(Pl0000, set2BBlackOut, void) // base + 0x4e7610
    OBJECT_SCRIPT_FUNCTION(Pl0000, set2BBreak, void) // base + 0x4e7640
    OBJECT_SCRIPT_FUNCTION(Pl0000, set2BBreakEvent, void) // base + 0x4e77b0
    OBJECT_SCRIPT_FUNCTION(Pl0000, set2BBreakWeak, void) // base + 0x4e7a70
    OBJECT_SCRIPT_FUNCTION(Pl0000, set2BDieFilterLv0, void) // base + 0x4e7aa0
    OBJECT_SCRIPT_FUNCTION(Pl0000, set2BDieFilterLv1, void) // base + 0x4e7ad0
    OBJECT_SCRIPT_FUNCTION(Pl0000, set2BDieFilterLv2, void) // base + 0x4e7b00
    OBJECT_SCRIPT_FUNCTION(Pl0000, set2BDieFilterLv3, void) // base + 0x4e7b30
    OBJECT_SCRIPT_FUNCTION(Pl0000, set2BDieSe00, void) // base + 0x4e7b60
    OBJECT_SCRIPT_FUNCTION(Pl0000, set2BDieSe10, void) // base + 0x4e7ba0
    OBJECT_SCRIPT_FUNCTION(Pl0000, set2BDieSe30, void) // base + 0x4e7be0
    OBJECT_SCRIPT_FUNCTION(Pl0000, set2BDieSe50, void) // base + 0x4e7c20
    OBJECT_SCRIPT_FUNCTION(Pl0000, set2BDieSe70, void) // base + 0x4e7c60
    OBJECT_SCRIPT_FUNCTION(Pl0000, set2BDieSe90, void) // base + 0x4e7ca0
    OBJECT_SCRIPT_FUNCTION(Pl0000, set2BPodShutterSe, void) // base + 0x4e7ce0
    OBJECT_SCRIPT_FUNCTION(Pl0000, set9sBreak, void) // base + 0x4e7d50
    OBJECT_SCRIPT_FUNCTION(Pl0000, set9sBreakWeak, void) // base + 0x4e7a70
    OBJECT_SCRIPT_FUNCTION(Pl0000, setA2Break, void) // base + 0x4e8180
    OBJECT_SCRIPT_FUNCTION(Pl0000, setA2BreakWeak, void) // base + 0x4e7a70
    OBJECT_SCRIPT_FUNCTION(Pl0000, setBattleWait, void) // base + 0x4e94a0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setBedSelfHackingRtn, void) // base + 0x4e9540
    OBJECT_SCRIPT_FUNCTION(Pl0000, setBedSit, void) // base + 0x4e9570
    OBJECT_SCRIPT_FUNCTION(Pl0000, setBedStandup, void) // base + 0x4e9600
    OBJECT_SCRIPT_FUNCTION(Pl0000, setBezierCurveMove, void, class app::Puid const &) // base + 0x4e9630
    OBJECT_SCRIPT_FUNCTION(Pl0000, setBuddyAiType, void, int) // base + 0x4e9cd0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setBuddyFlightUnit, void) // base + 0x4e9db0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setBuddyFromNpc, void) // base + 0x4e9e10
    OBJECT_SCRIPT_FUNCTION(Pl0000, setBunkerEventEnd, void) // base + 0x4ea5e0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setBunkerEventStart, void) // base + 0x4ea610
    OBJECT_SCRIPT_FUNCTION(Pl0000, setCamReset, void) // base + 0x4ea640
    OBJECT_SCRIPT_FUNCTION(Pl0000, setCameraHokanSpd, void, int) // base + 0x4ea650
    OBJECT_SCRIPT_FUNCTION(Pl0000, setChipPreset, void, int) // base + 0x4ea730
    OBJECT_SCRIPT_FUNCTION(Pl0000, setControlHackReturnPos, void, const Vector4f&) // base + 0x4eab40
    OBJECT_SCRIPT_FUNCTION(Pl0000, setCorpse, void) // base + 0x4eab50
    OBJECT_SCRIPT_FUNCTION(Pl0000, setDownEmp, void) // base + 0x4ec1a0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setDownVirus, void) // base + 0x4ec1e0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setDownWait, void) // base + 0x4ec210
    OBJECT_SCRIPT_FUNCTION(Pl0000, setDownWakeUp, void) // base + 0x4ec240
    OBJECT_SCRIPT_FUNCTION(Pl0000, setE3TimeTrial, void, int) // base + 0x4ec270
    OBJECT_SCRIPT_FUNCTION(Pl0000, setEm4000EvGetOff, void) // base + 0x4ec6a0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setEm4000EvReturn, void) // base + 0x4ec6d0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setEventMuteki, void) // base + 0x4eca50
    OBJECT_SCRIPT_FUNCTION(Pl0000, setFlightUnitBlack, void) // base + 0x4ed190
    OBJECT_SCRIPT_FUNCTION(Pl0000, setFlightUnitEff2BBomb, void) // base + 0x4ed210
    OBJECT_SCRIPT_FUNCTION(Pl0000, setFlightUnitEff2BBombSml, void) // base + 0x4ed380
    OBJECT_SCRIPT_FUNCTION(Pl0000, setFlightUnitEff2BDamage, void) // base + 0x4ed4c0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setFlightUnitEff2BStealthOff, void) // base + 0x4ed640
    OBJECT_SCRIPT_FUNCTION(Pl0000, setFlightUnitEff2Bto9S, void) // base + 0x4ed730
    OBJECT_SCRIPT_FUNCTION(Pl0000, setFlightUnitWhite, void) // base + 0x4ed920
    OBJECT_SCRIPT_FUNCTION(Pl0000, setForceEmpNoise, void) // base + 0x4ee0d0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setGoggleOff, void) // base + 0x4ee1e0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingDownLoop, void) // base + 0x4ee2f0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingFilterCore11, void) // base + 0x4ee3c0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingFilterCore12, void) // base + 0x4ee3f0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingFilterCore13, void) // base + 0x4ee420
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingFilterCore14, void) // base + 0x4ee450
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingFilterCore17, void) // base + 0x4ee480
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingFilterMono, void) // base + 0x4ee4f0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingFilterReset, void) // base + 0x4ee520
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingFilterUI, void) // base + 0x4ee550
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingLightComp, void) // base + 0x4ee560
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingLightDelete, void) // base + 0x4ee6b0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingLightL, void) // base + 0x4ee6d0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingLightM, void) // base + 0x4ee7f0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingLightS, void) // base + 0x4ee910
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingLocation, void, const Vector4f&) // base + 0x4eea30
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingSound_Access_1, void) // base + 0x4eece0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingSound_Access_2, void) // base + 0x4eed10
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingSound_Access_3, void) // base + 0x4eed40
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingSound_Mute, void) // base + 0x4eed70
    OBJECT_SCRIPT_FUNCTION(Pl0000, setHackingTopView, void) // base + 0x4eede0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setInputIgnore, void, float) // base + 0x4ef190
    OBJECT_SCRIPT_FUNCTION(Pl0000, setNPC2BArtillery, void) // base + 0x4efab0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setNPCShootingFighter, void) // base + 0x4efb50
    OBJECT_SCRIPT_FUNCTION(Pl0000, setNPCShootingHuman, void) // base + 0x4efb80
    OBJECT_SCRIPT_FUNCTION(Pl0000, setNPCShootingPosOutTimer, void) // base + 0x4efbb0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setNpcAtkWait, void) // base + 0x4efe00
    OBJECT_SCRIPT_FUNCTION(Pl0000, setNpcAtkWaitOnly, void) // base + 0x4efe20
    OBJECT_SCRIPT_FUNCTION(Pl0000, setNpcBattle, void) // base + 0x4efe30
    OBJECT_SCRIPT_FUNCTION(Pl0000, setNpcFromBuddy, void) // base + 0x4f0160
    OBJECT_SCRIPT_FUNCTION(Pl0000, setPlayerFromNPC, void) // base + 0x4f0ca0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setPosResetHap, void, const Vector4f&) // base + 0x4f15b0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setPosRotResetHap, void, const Vector4f&, const glm::quat&) // base + 0x4f1620
    OBJECT_SCRIPT_FUNCTION(Pl0000, setPosTowerLift, void) // base + 0x4f1640
    OBJECT_SCRIPT_FUNCTION(Pl0000, setRobotRescueStartLocation, void, const Vector4f&, float const &) // base + 0x4f25a0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setRobotRescueWakeUp, void) // base + 0x4f25d0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setRunTarget, void, const Vector4f&) // base + 0x4f2650
    OBJECT_SCRIPT_FUNCTION(Pl0000, setSelfHacking, void) // base + 0x4f2930
    OBJECT_SCRIPT_FUNCTION(Pl0000, setSelfHackingDamage, void) // base + 0x4f2970
    OBJECT_SCRIPT_FUNCTION(Pl0000, setShootingCamBehind, void) // base + 0x4f4ac0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setShootingCamQuater, void) // base + 0x4f4ae0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setShootingCamSide, void) // base + 0x4f4b00
    OBJECT_SCRIPT_FUNCTION(Pl0000, setShootingCamTop, void) // base + 0x4f4b20
    OBJECT_SCRIPT_FUNCTION(Pl0000, setShootingFrontOver, void) // base + 0x4f4c60
    OBJECT_SCRIPT_FUNCTION(Pl0000, setShootingOverStop, void) // base + 0x4f4ea0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setShootingPosResetTimer, void) // base + 0x4efbb0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setShootingPosUpResetTimer, void) // base + 0x4f5300
    OBJECT_SCRIPT_FUNCTION(Pl0000, setShootingSideOver, void) // base + 0x4f5310
    OBJECT_SCRIPT_FUNCTION(Pl0000, setShootingSideOverSlow, void) // base + 0x4f5320
    OBJECT_SCRIPT_FUNCTION(Pl0000, setSleepBed, void) // base + 0x4f5680
    OBJECT_SCRIPT_FUNCTION(Pl0000, setStateMosaic, void) // base + 0x4f5df0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setTowerShooting, void, const Vector4f&) // base + 0x4f5ff0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setTurnTarget, void, const Vector4f&) // base + 0x4f6210
    OBJECT_SCRIPT_FUNCTION(Pl0000, setUINoiseFilterLv0, void) // base + 0x4f6250
    OBJECT_SCRIPT_FUNCTION(Pl0000, setUINoiseFilterLv1, void) // base + 0x4f6280
    OBJECT_SCRIPT_FUNCTION(Pl0000, setUINoiseFilterLv2, void) // base + 0x4f62b0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setUINoiseFilterLv3, void) // base + 0x4f62e0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setUINoiseFilterMax, void) // base + 0x4f6310
    OBJECT_SCRIPT_FUNCTION(Pl0000, setUIPollutionGaugeLv1, void, float) // base + 0x4f6330
    OBJECT_SCRIPT_FUNCTION(Pl0000, setUIPollutionGaugeLv2, void, float) // base + 0x4f6370
    OBJECT_SCRIPT_FUNCTION(Pl0000, setUIPollutionGaugeLv3, void, float) // base + 0x4f63b0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setUIPollutionGaugeLv4, void, float) // base + 0x4f63f0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setUIPollutionGaugeLv5, void, float) // base + 0x4f6430
    OBJECT_SCRIPT_FUNCTION(Pl0000, setUIPollutionGaugeLv6, void, float) // base + 0x4f6470
    OBJECT_SCRIPT_FUNCTION(Pl0000, setUIPollutionGaugeLv7, void, float) // base + 0x4f64b0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setVirusDamage, void) // base + 0x4f6520
    OBJECT_SCRIPT_FUNCTION(Pl0000, setVirusMode, void) // base + 0x4f65f0
    OBJECT_SCRIPT_FUNCTION(Pl0000, setWait, void) // base + 0x4f6620
    OBJECT_SCRIPT_FUNCTION(Pl0000, setWakeUpBed, void) // base + 0x4f6640
    OBJECT_SCRIPT_FUNCTION(Pl0000, setWalkTarget, void, const Vector4f&) // base + 0x4f6670
    OBJECT_SCRIPT_FUNCTION(Pl0000, setYorhaBackFoot, void) // base + 0x4f66a0
    OBJECT_SCRIPT_FUNCTION(Pl0000, shotNPC2BArtillery, void) // base + 0x4f6d40
    OBJECT_SCRIPT_FUNCTION(Pl0000, start2BDefence, void) // base + 0x4f6fa0
    OBJECT_SCRIPT_FUNCTION(Pl0000, start2BDefenceSuffer, void) // base + 0x4f6fe0
    OBJECT_SCRIPT_FUNCTION(Pl0000, start2BMoveTo9S, void) // base + 0x4f7010
    OBJECT_SCRIPT_FUNCTION(Pl0000, start9SDefence, void, const Vector4f&) // base + 0x4f7040
    OBJECT_SCRIPT_FUNCTION(Pl0000, start9SDefenceUpload, void) // base + 0x4f7070
    OBJECT_SCRIPT_FUNCTION(Pl0000, start9SEm3000Hacking, void) // base + 0x4f7080
    OBJECT_SCRIPT_FUNCTION(Pl0000, start9SEveHacking, void) // base + 0x4f70c0
    OBJECT_SCRIPT_FUNCTION(Pl0000, start9sLast, void) // base + 0x4f7110
    OBJECT_SCRIPT_FUNCTION(Pl0000, start9sMoveTo2B, void) // base + 0x4f7210
    OBJECT_SCRIPT_FUNCTION(Pl0000, start9sRuinedCityGoOff, void) // base + 0x4f7270
    OBJECT_SCRIPT_FUNCTION(Pl0000, start9sRuinedCityWarp, void, const Vector4f&) // base + 0x4f7280
    OBJECT_SCRIPT_FUNCTION(Pl0000, startA2Tired, void) // base + 0x4f73c0
    OBJECT_SCRIPT_FUNCTION(Pl0000, startAirShipBoss, void) // base + 0x4f7410
    OBJECT_SCRIPT_FUNCTION(Pl0000, startChangePLNoise, void) // base + 0x4f7430
    OBJECT_SCRIPT_FUNCTION(Pl0000, startEventConnect, void) // base + 0x4f7890
    OBJECT_SCRIPT_FUNCTION(Pl0000, startEventWeapon, void) // base + 0x4f7950
    OBJECT_SCRIPT_FUNCTION(Pl0000, startForceAttackEnable, void) // base + 0x4f8ab0
    OBJECT_SCRIPT_FUNCTION(Pl0000, startGoAway, void) // base + 0x4f8ae0
    OBJECT_SCRIPT_FUNCTION(Pl0000, startHackingQuestBugNoise, void) // base + 0x4f8d30
    OBJECT_SCRIPT_FUNCTION(Pl0000, startLaserKill, void) // base + 0x4f8d60
    OBJECT_SCRIPT_FUNCTION(Pl0000, startNoDmgSubQuest, void) // base + 0x4f8e20
    OBJECT_SCRIPT_FUNCTION(Pl0000, startPhaseEventFlightUnit, void) // base + 0x4f8e30
    OBJECT_SCRIPT_FUNCTION(Pl0000, startPhaseEventFlightUnitNoWep, void) // base + 0x4f8e40
    OBJECT_SCRIPT_FUNCTION(Pl0000, startPodMoveMode, void) // base + 0x4f9020
    OBJECT_SCRIPT_FUNCTION(Pl0000, startShootingNPCMove, void) // base + 0x4fa7c0
    OBJECT_SCRIPT_FUNCTION(Pl0000, startShootingP100Robot, void) // base + 0x4fa7d0
    OBJECT_SCRIPT_FUNCTION(Pl0000, startTo9sRun, void) // base + 0x4fb200
    OBJECT_SCRIPT_FUNCTION(Pl0000, startTraining, void) // base + 0x4fb230
    OBJECT_SCRIPT_FUNCTION(Pl0000, startVirusLv1, void) // base + 0x4fb240
    OBJECT_SCRIPT_FUNCTION(Pl0000, startVirusLv2, void) // base + 0x4fb260
    OBJECT_SCRIPT_FUNCTION(Pl0000, startWeakMode, void) // base + 0x4fb290

private:
    uint8_t detail_pl0000_data[sizeof(regenny::Pl0000) - sizeof(BehaviorAppBase)];
};

static_assert(sizeof(Pl0000) == 0x17920, "Pl0000 size mismatch");
}