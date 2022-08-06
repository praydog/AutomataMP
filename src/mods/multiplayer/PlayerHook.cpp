#include <intrin.h>

#include <spdlog/spdlog.h>

#include <sdk/Entity.hpp>
#include <sdk/EntityList.hpp>
#include "mods/AutomataMPMod.hpp"
#include "PlayerHook.hpp"

using namespace std;

unordered_set<uint32_t> g_unreplicatedAnims {
    EAnimation::Light_Attack,
    EAnimation::Dash
};

PlayerHook* g_playerHook = nullptr;

PlayerHook::PlayerHook() {
    g_playerHook = this;
}

void PlayerHook::reHook(Entity* player) {
    if (m_hook.getInstance().as<Entity*>() == player) {
        return;
    }

    if (m_hook.create(player)) {
        m_hook.hookMethod(Entity::s_startAnimationIndex, &startAnimationHook);
    }
}

void __thiscall PlayerHook::startAnimationHook(Entity* ent, uint32_t anim, uint32_t variant, uint32_t a3, uint32_t a4) {
    if (g_unreplicatedAnims.count(anim) == 0) {
        auto amp = AutomataMPMod::get();
        auto& client = amp->getClient();

        if (client != nullptr) {
            client->sendAnimationStart(anim, variant, a3, a4);
        }
    }

    spdlog::info("anim: {}, variant: {}, a3: {}, return: {:x}", anim, variant, a3, (uintptr_t)_ReturnAddress());

    auto original = g_playerHook->getHook().getMethod<decltype(startAnimationHook)*>(Entity::s_startAnimationIndex);
    original(ent, anim, variant, a3, a4);
}
