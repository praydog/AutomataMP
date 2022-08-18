#include "scene_state/SceneStateSystem.hpp"
#include "StateObject.hpp"

namespace sdk::hap {
StateObject* get_first() {
    return scene_state::SceneStateSystem::get();
}

StateObject* find(std::string_view name) {
    auto first = get_first();

    if (first == nullptr) {
        return nullptr;
    }

    for (auto state = first; state != nullptr; state = state->next_state()) {
        if (state->state_name() == name) {
            return state;
        }
    }
    
    return nullptr;
}
}