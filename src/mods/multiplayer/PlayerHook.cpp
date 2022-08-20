#include <intrin.h>

#include <spdlog/spdlog.h>

#include <sdk/Entity.hpp>
#include <sdk/EntityList.hpp>
#include <sdk/Enums.hpp>
#include "mods/AutomataMPMod.hpp"
#include "PlayerHook.hpp"

using namespace std;

unordered_set<uint32_t> g_unreplicated_anims{sdk::EAnimation::Light_Attack, sdk::EAnimation::Dash};

PlayerHook* g_player_hook = nullptr;

PlayerHook::PlayerHook() {
    g_player_hook = this;
}

void PlayerHook::re_hook(sdk::Pl0000* player) {
    if (m_hook.get_instance().as<sdk::Pl0000*>() == player) {
        return;
    }

    if (m_hook.create(player)) {
        m_hook.hook_method(sdk::Behavior::s_start_animation_index, &start_animation_hook);
    }
}

void __thiscall PlayerHook::start_animation_hook(sdk::Pl0000* ent, uint32_t anim, uint32_t variant, uint32_t a3, uint32_t a4) {
    if (g_unreplicated_anims.count(anim) == 0) {
        auto amp = AutomataMPMod::get();
        auto& client = amp->get_client();

        if (client != nullptr) {
            client->send_animation_start(anim, variant, a3, a4);
        }
    }

    spdlog::info("anim: {}, variant: {}, a3: {}, return: {:x}", anim, variant, a3, (uintptr_t)_ReturnAddress());

    auto original = g_player_hook->get_hook().get_method<decltype(start_animation_hook)*>(sdk::Behavior::s_start_animation_index);
    original(ent, anim, variant, a3, a4);
}
