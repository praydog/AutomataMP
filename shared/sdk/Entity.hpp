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
    void assign_ai_routine(const std::string& name);

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
}