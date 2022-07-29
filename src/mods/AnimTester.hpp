#pragma once

#include "Mod.hpp"

class AnimTester : public Mod {
public:
    std::string_view get_name() const override { return "AnimTester"; }

    void on_draw_ui() override;

private:
    ModInt32::Ptr m_anim{ ModInt32::create("Animation Type") };
    ModInt32::Ptr m_variant { ModInt32::create("Animation Variant") };
    ModInt32::Ptr m_a3 { ModInt32::create("a3") };
    ModInt32::Ptr m_a4 { ModInt32::create("a4") };
};