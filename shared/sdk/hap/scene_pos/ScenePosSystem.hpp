#pragma once

#include "../StateObject.hpp"

namespace sdk::hap::scene_pos {
class ScenePosSystem : public ::sdk::hap::StateObject {
public:

private:
    uint8_t detail_scene_pos_system[0x70 - sizeof(::sdk::hap::StateObject)];
};

static_assert(sizeof(ScenePosSystem) == 0x70, "Size of ScenePosSystem is not correct.");
}