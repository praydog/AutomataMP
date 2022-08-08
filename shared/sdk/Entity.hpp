#pragma once

#include <cstdint>

#include <utility/Address.hpp>
#include <utility/RTTI.hpp>
#include <sdk/Math.hpp>

#include "ScriptFunctions.hpp"

#include "Behavior.hpp"
#include "Pl0000.hpp"

namespace sdk {
class Entity;

struct Entity {
    void assignAIRoutine(const std::string& name);

    void* unknown;
    char name[0x20];
    uint64_t flags;
    uint32_t handle;
    void* unknown2;
    void* unknown3;
    sdk::Behavior* behavior;
    void* unknown4;
    char poop[0x8];
    sdk::Behavior* behavior2;
};

enum EAnimation {
    Walk = 1,
    Run = 2,
    Sprint = 3,
    High_Fall = 4,
    Jump = 5,
    Fall = 6,
    High_Fall2 = 7,
    Air_Spin = 9,
    Land = 11,
    Get_Up_Backflip = 12,
    Backflip = 13,
    Vault = 14,
    Lying_Down = 15,
    Light_Attack = 16,
    Dash = 18,
    Dodge = 19,
    Taunt_A2 = 20,
    Parry_thing = 21,
    Electrocuted = 22,
    Taunt_A2_2 = 23,
    Throw_Weapon = 24,
    Slomo_Cutscene = 25,
    Really_Powerful_Kick_2B = 26,
    INVALID_CRASHES_GAME = 27,
    Use_Corpse = 28,
    Use_Corpse_2 = 29,
    Use_Corpse_3 = 30,
    Pick_Up_Item = 31,
    Pervert_2B = 32,
    Pick_Up_Item_2 = 33,
    Pod_Fire_Rocket = 36,
    Pod_Swing = 37,
    Flying_Thing = 38,
    Flying_Mech_Mode_Bugged = 39,
    Push_Back_Weak = 42,
    Push_Back_Weak_2 = 43,
    Push_Back_Medium = 44,
    Push_Back_Hard = 45,
    Push_Back_Fall = 46,
    Push_Box = 48,
    Lift_Up_Fall = 51,
    Die = 53,
    Die_2 = 54,
    Self_Destruct_Start = 55,
    // does a countdown and actually blows up
    Self_Destruct_Complete = 56,
    Self_Destruct_Explode = 57,
    // Slides with anything other than variant 0
    Slide = 58,
    Turn_90_Deg_Right = 63,
    INVALID_CRASHES_GAME2 = 65,
    Hacking_Mode = 70,
    Walk_Forward_Scripted = 85,
    Flying_Mech_Mode = 94,
    Land_Mechs = 95,
    Open_Door = 102,
    Sit_Down = 103,
    INVALID_CRASHES_GAME3 = 104,
    Kick = 105,
    INVALID_CRASHES_GAME4 = 106,
    Lie_In_Bed = 107,
};

enum EModel {
    MODEL_2B = 0x10000,
    MODEL_A2 = 0x10100,
    MODEL_9S = 0x10200,
};

enum ERunSpeedType {
    SPEED_PLAYER,
    SPEED_BUDDY
};

class Entity_ {
public:
    struct CharacterController {
        enum EButtonIndex {
            INDEX_ATTACK_LIGHT = 0,
            INDEX_ATTACK_HEAVY = 1,
            INDEX_JUMP = 2,
            INDEX_POD_SPECIAL = 4,
            INDEX_POD_FIRE = 6,
            INDEX_DASH = 7,
            INDEX_MAX = INDEX_DASH + 1,
        };

        char crap[0x738];
        uint32_t heldFlags;
        uint32_t flags;
        char unknown[0x30];

        uint32_t buttons[EButtonIndex::INDEX_MAX];
    };

public:
    bool isNetworkable() const {
        return utility::rtti::derives_from(this, "class EmBase") ||
               utility::rtti::derives_from(this, "class BaAnimal");
    }

    uint32_t* getModel() {
        return (uint32_t*)((uintptr_t)this + 0x5B8);
    }

    Entity* getContainer() {
        return *(Entity**)((uintptr_t)this + 0x610);
    }

    uint32_t* getHealth() {
        return (uint32_t*)((uintptr_t)this + 0x858);
    }

    uint32_t getPossessedHandle() {
        return *(uint32_t*)((uintptr_t)this + 0x12A5C);
    }

    void setBuddyHandle(uint32_t handle) {
        *(uint32_t*)((uintptr_t)this + 0x1646c + 0x10) = handle; // + 0x10 because of new version.
    }

    uint32_t getBuddyHandle() {
        return *(uint32_t*)((uintptr_t)this + 0x1646c + 0x10);
    }

    uint32_t getBuddyFlags() const {
        return *(uint32_t*)((uintptr_t)this + 0x598);
    }

    void setBuddyFlags(uint32_t flags) {
        *(uint32_t*)((uintptr_t)this + 0x598) = flags;
    }

    CharacterController* getCharacterController() {
        return (CharacterController*)((uintptr_t)this + 0xCA0);
    }

    float* getSpeed() {
        return Address(getCharacterController()).get(0x794).as<float*>();
    }

    float* getFacing() {
        return Address(this).get(0x94).as<float*>();
    }

    float* getFacing2() {
        return Address(getCharacterController()).get(0x79C).as<float*>();
    }

    Vector3f* getPosition() {
        return Address(this).get(0x50).as<Vector3f*>();
    }

    uint32_t* getWeaponIndex() {
        return Address(this).get(0x107d8).as<uint32_t*>();
    }

    uint32_t* getPodIndex() {
        return Address(this).get(0x10820).as<uint32_t*>();
    }

    bool* getFlashlightEnabled() {
        return Address(this).get(0x17480 + 0x30).as<bool*>();
    }

    float* getTickCount() {
        return Address(this).get(0x9C).as<float*>();
    }

    bool* getShortsTorn() {
        return Address(this).get(0x16ce4 + 0x10).as<bool*>();
    }

    bool* getWearingMask() {
        return Address(this).get(0x16cdc + 0x10).as<bool*>();
    }

    uint32_t* getRunSpeedType() {
        return (uint32_t*)((uintptr_t)this + 0x16478 + 0x10);
    }

    struct Signal {
        uint32_t signal;
        uint32_t unk{ 0 };
        uintptr_t unk2{ 0 };
        uint32_t unk3{ 0 };
    };

    OBJECT_SCRIPT_FUNCTION(Behavior, terminate, void)
    OBJECT_SCRIPT_FUNCTION(Behavior, isSuspend, bool);
    OBJECT_SCRIPT_FUNCTION(Behavior, setSuspend, void, bool);
    OBJECT_SCRIPT_FUNCTION(Behavior, signal, void, const Signal&);
    OBJECT_SCRIPT_FUNCTION(Behavior, offCollision, void);
    OBJECT_SCRIPT_FUNCTION(Behavior, onCollision, void);
    OBJECT_SCRIPT_FUNCTION(Behavior, onTrans, void);
    OBJECT_SCRIPT_FUNCTION(Behavior, offTrans, void);
    OBJECT_SCRIPT_FUNCTION(Behavior, isTrans, void);

    OBJECT_SCRIPT_FUNCTION(Pl0000, setBuddyFromNpc, void);
    OBJECT_SCRIPT_FUNCTION(Pl0000, changePlayer, void);
    OBJECT_SCRIPT_FUNCTION(Pl0000, setPosRotResetHap, void, const Vector4f&, const glm::quat&);

private:
};
}