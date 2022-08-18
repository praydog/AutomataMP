#pragma once

#include <cstdint>
#include <string_view>

#include <utility/RTTI.hpp>

#include "regenny/Behavior.hpp"
#include "Obj.hpp"
#include "lib/HashedString.hpp"
#include "hap/State.hpp"
#include "ScriptFunctions.hpp"

namespace sdk {
class Entity;

class Behavior : public sdk::Obj {
public:
    static constexpr std::string_view class_name() {
        return "class Behavior";
    }

public:
    virtual void pad4() = 0;
    virtual void pad5() = 0;
    virtual void pad6() = 0;
    virtual void pad7() = 0;
    virtual void pad8() = 0;
    virtual void pad9() = 0;
    virtual void pad10() = 0;
    virtual void pad11() = 0;
    virtual void pad12() = 0;
    virtual void pad13() = 0;
    virtual void pad14() = 0;
    virtual void pad15() = 0;
    virtual void pad16() = 0;
    virtual void pad17() = 0;
    virtual void start_animation(uint32_t animType, uint32_t variant, uint32_t a3 = 0, uint32_t a4 = 0) = 0;

public:
    ::regenny::Behavior* regenny() const {
        return (::regenny::Behavior*)this;
    }

    Entity* get_entity() const {
        return (Entity*)regenny()->entity;
    }

    bool is_networkable() const {
        return utility::rtti::derives_from(this, "class EmBase") ||
               utility::rtti::derives_from(this, "class BaAnimal");
    }

public:
    OBJECT_SCRIPT_FUNCTION(Behavior, clearHackingStageLabel, void) // base + 0x480190
    OBJECT_SCRIPT_FUNCTION(Behavior, eraseSave, bool) // base + 0x48a2a0
    OBJECT_SCRIPT_FUNCTION(Behavior, isSuspend, int) // base + 0x20e170
    OBJECT_SCRIPT_FUNCTION(Behavior, isTrans, int) // base + 0x20e160
    OBJECT_SCRIPT_FUNCTION(Behavior, offCollision, void) // base + 0x464300
    OBJECT_SCRIPT_FUNCTION(Behavior, offTrans, void) // base + 0x20e180
    OBJECT_SCRIPT_FUNCTION(Behavior, onCollision, void) // base + 0x46430c
    OBJECT_SCRIPT_FUNCTION(Behavior, onTrans, void) // base + 0x20e0c0
    OBJECT_SCRIPT_FUNCTION(Behavior, setHackingStageLabelByHap, void, class lib::HashedString<struct sys::StringSystem::Allocator> const &) // base + 0x4eedb0
    OBJECT_SCRIPT_FUNCTION(Behavior, setSuspend, void, int) // base + 0x464318
    OBJECT_SCRIPT_FUNCTION(Behavior, signal, bool, class hap::State const &) // base + 0x4f6e80
    OBJECT_SCRIPT_FUNCTION(Behavior, terminate, void) // base + 0x4f5fd0

    // Vtable indices
    static constexpr uint8_t s_start_animation_index = 18;

private:
    uint8_t detail_behavior_data[sizeof(::regenny::Behavior) - sizeof(Obj)];
};

static_assert(sizeof(Behavior) == 0x830, "Size of Behavior is not correct.");
}