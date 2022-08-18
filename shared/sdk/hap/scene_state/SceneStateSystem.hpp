#pragma once

#include "../StateObject.hpp"
#include "../TokenCategory.hpp"
#include "../SceneStateName.hpp"
#include "../../sys/StringSystem.hpp"
#include "../../lib/HashedString.hpp"
#include "../../ScriptFunctions.hpp"

namespace sdk::hap::scene_state {
class SceneStateSystem : public ::sdk::hap::StateObject, public ::sdk::hap::TokenCategory {
public:
    static SceneStateSystem* get();

public:
    OBJECT_SCRIPT_FUNCTION(SceneStateSystem, clear, void) // base + 0x321cc0
    OBJECT_SCRIPT_FUNCTION(SceneStateSystem, clearGroup, void, class lib::HashedString<struct sys::StringSystem::Allocator> const &) // base + 0x321cf0
    OBJECT_SCRIPT_FUNCTION(SceneStateSystem, has, bool, struct hap::SceneStateName const &) // base + 0x33dea0
    OBJECT_SCRIPT_FUNCTION(SceneStateSystem, reset, bool, struct hap::SceneStateName const &) // base + 0x34f7e0
    OBJECT_SCRIPT_FUNCTION(SceneStateSystem, set, bool, struct hap::SceneStateName const &) // base + 0x354a20
};
}