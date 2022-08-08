#pragma once

#include <utility/VtableHook.hpp>

namespace sdk {
class Pl0000;
}

class PlayerHook {
public:
    PlayerHook();

    void reHook(sdk::Pl0000* player);

    auto& getHook() {
        return m_hook;
    }

public:
    static void __thiscall startAnimationHook(sdk::Pl0000* ent, uint32_t anim, uint32_t variant, uint32_t a3, uint32_t a4);

private:
    VtableHook m_hook;
};