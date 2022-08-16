#pragma once

#include <cstdint>
#include <string_view>

#include <utility/RTTI.hpp>

#include "regenny/Behavior.hpp"
#include "Obj.hpp"
#include "ScriptFunctions.hpp"

namespace sdk {
class Entity;

class Behavior : public sdk::Obj {
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
    OBJECT_SCRIPT_FUNCTION(Behavior, isTrans, bool);
    OBJECT_SCRIPT_FUNCTION(Behavior, clearHackingStageLabel, void);
    OBJECT_SCRIPT_FUNCTION(Behavior, setHackingStageLabelByHap, void, void*);

    // Vtable indices
    static constexpr uint8_t s_start_animation_index = 18;

private:
    uint8_t detail_behavior_data[sizeof(::regenny::Behavior) - sizeof(Obj)];
};

static_assert(sizeof(Behavior) == 0x830, "Size of Behavior is not correct.");
}