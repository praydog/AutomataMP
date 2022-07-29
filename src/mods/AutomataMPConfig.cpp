#include "AutomataMP.hpp"

#include "AutomataMPConfig.hpp"

std::shared_ptr<AutomataMPConfig>& AutomataMPConfig::get() {
     static std::shared_ptr<AutomataMPConfig> instance{std::make_shared<AutomataMPConfig>()};
     return instance;
}

std::optional<std::string> AutomataMPConfig::on_initialize() {
    return Mod::on_initialize();
}

void AutomataMPConfig::on_draw_ui() {
    if (!ImGui::CollapsingHeader("Configuration")) {
        return;
    }

    ImGui::TreePush("Configuration");

    //m_menu_key->draw("Menu Key");
    m_remember_menu_state->draw("Remember Menu Open/Closed State");

    if (m_font_size->draw("Font Size")) {
        g_framework->set_font_size(m_font_size->value());
    }

    ImGui::TreePop();
}

void AutomataMPConfig::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }

    if (m_remember_menu_state->value()) {
        g_framework->set_draw_ui(m_menu_open->value(), false);
    }
    
    g_framework->set_font_size(m_font_size->value());
}

void AutomataMPConfig::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}
