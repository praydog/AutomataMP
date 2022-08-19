#pragma once

#include "../Mod.hpp"

namespace sdk {
class Behavior;
class EmBase;
class Pl0000;
}

class Explorer : public Mod {
public:
    std::string_view get_name() const override {
        return "Explorer";
    }

    void on_draw_ui() override;

private:
    void display_player_options();
    void display_entities();
    void display_behavior(sdk::Behavior* behavior, sdk::Behavior* player_behavior = nullptr);
    void display_em_base(sdk::EmBase* em, sdk::Behavior* player_behavior = nullptr);
    void display_pl0000(sdk::Pl0000* pl0000);
};