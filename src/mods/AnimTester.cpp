#include <sdk/Entity.hpp>
#include <sdk/EntityList.hpp>

#include "AnimTester.hpp"

void AnimTester::on_draw_ui() {
    if (!ImGui::CollapsingHeader("AnimTester")) {
        return;
    }

    if (ImGui::Button("Start Animation")) {
        auto entityList = sdk::EntityList::get();

        if (!entityList) {
            return;
        }

        auto player = entityList->get_by_name("Player");
        auto partner = entityList->get_by_name("partner");

        if (player && player->behavior) {
            player->behavior->start_animation(m_anim->value(), m_variant->value(), m_a3->value(), m_a4->value());
        }

        if (partner && partner->behavior) {
            partner->behavior->start_animation(m_anim->value(), m_variant->value(), m_a3->value(), m_a4->value());
        }
    }

    m_anim->draw("Animation");
    m_variant->draw("Variant");
    m_a3->draw("a3");
    m_a4->draw("a4");
}
