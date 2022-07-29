#pragma once

#include "Mod.hpp"

class BuddyFeatures : public Mod {
public:
    BuddyFeatures();

    std::string_view get_name() const override { return "BuddyFeatures"; }
    void on_think() override;
    
private:
    ModToggle::Ptr m_disable_ai{ ModToggle::create("Disable AI") };
};