#pragma once

#include <utility/VtableHook.hpp>

class Entity;

class PlayerHook {
public:
    PlayerHook();

    void reHook(Entity* player);

    auto& getHook() {
        return m_hook;
    }

public:
    static void __thiscall startAnimationHook(Entity* ent, uint32_t anim, uint32_t variant, uint32_t a3, uint32_t a4);

private:
    VtableHook m_hook;
};