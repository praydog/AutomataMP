#include <sdk/Entity.hpp>
#include <sdk/EntityList.hpp>

#include "AnimTester.hpp"

void AnimTester::on_draw_ui() {
    if (!ImGui::CollapsingHeader("AnimTester")) {
        return;
    }

    if (ImGui::Button("Start Animation")) {
        auto entityList = EntityList::get();

        if (!entityList) {
            return;
        }

        auto player = entityList->getByName("Player");
        auto partner = entityList->getByName("partner");

        if (player && player->entity) {
            player->entity->startAnimation(m_anim->value(), m_variant->value(), m_a3->value(), m_a4->value());
        }

        if (partner && partner->entity) {
            partner->entity->startAnimation(m_anim->value(), m_variant->value(), m_a3->value(), m_a4->value());
        }
    }

    m_anim->draw("Animation");
    m_variant->draw("Variant");
    m_a3->draw("a3");
    m_a4->draw("a4");
}
