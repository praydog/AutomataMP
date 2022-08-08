#pragma once

#include <utility/RTTI.hpp>

#include "Math.hpp"
#include "regenny/Pl0000.hpp"
#include "BehaviorAppBase.hpp"

#include "ScriptFunctions.hpp"

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

    __forceinline ::regenny::Pl0000* as_pl0000() const {
        return (::regenny::Pl0000*)this;
    }

    __forceinline uint32_t& buddy_handle() {
        return as_pl0000()->buddy_handle;
    }

    __forceinline regenny::CharacterController& character_controller() {
        return as_pl0000()->controller;
    }

    __forceinline float& facing2() {
        return character_controller().facing;
    }

    __forceinline uint32_t& weapon_index() {
        return as_pl0000()->weapon_index;
    }

    __forceinline uint32_t& pod_index() {
        return as_pl0000()->pod_index;
    }

    __forceinline bool& flashlight() {
        return as_pl0000()->flashlight;
    }

    __forceinline auto& run_speed_type() {
        return as_pl0000()->run_speed_type;
    }

    __forceinline float& speed() {
        return character_controller().speed;
    }

public:
    OBJECT_SCRIPT_FUNCTION(Pl0000, setBuddyFromNpc, void);
    OBJECT_SCRIPT_FUNCTION(Pl0000, changePlayer, void);
    OBJECT_SCRIPT_FUNCTION(Pl0000, setPosRotResetHap, void, const Vector4f&, const glm::quat&);

private:
    uint8_t detail_pl0000_data[sizeof(regenny::Pl0000) - sizeof(BehaviorAppBase)];
};

static_assert(sizeof(Pl0000) == 0x17920, "Pl0000 size mismatch");
}