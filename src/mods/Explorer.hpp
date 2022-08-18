#pragma once

#include "../Mod.hpp"

class Explorer : public Mod {
public:
    std::string_view get_name() const override {
        return "Explorer";
    }

    void on_draw_ui() override;

private:
    void display_entities();
};