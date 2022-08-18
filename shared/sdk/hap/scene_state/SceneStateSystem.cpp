#include <spdlog/spdlog.h>

#include "../TokenCategory.hpp"
#include "SceneStateSystem.hpp"

namespace sdk::hap::scene_state {
SceneStateSystem* SceneStateSystem::get() {
    static auto category = []() -> ::sdk::hap::TokenCategory* {
        spdlog::info("Finding scene state system...");

        auto scene_system = ::sdk::hap::TokenCategory::find("@SceneState");

        if (scene_system == nullptr) {
            spdlog::error("Failed to find scene state system.");
            return nullptr;
        }

        return scene_system;
    }();

    // - 0x40 essentially.
    return static_cast<SceneStateSystem*>(category);
}
}