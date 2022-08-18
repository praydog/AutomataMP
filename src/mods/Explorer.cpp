#include <unordered_map>
#include <map>

#include <imgui.h>
#include <utility/RTTI.hpp>

#include <sdk/Behavior.hpp>
#include <sdk/EntityList.hpp>
#include <sdk/Entity.hpp>
#include "Explorer.hpp"

void Explorer::on_draw_ui() {
    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    if (ImGui::TreeNode("Entities")) {
        display_entities();
        ImGui::TreePop();
    }
}

void Explorer::display_entities() {
    std::map<std::string, std::vector<sdk::Behavior*>> behavior_map{};
    const auto entities = sdk::EntityList::get();

    if (entities == nullptr) {
        return;
    }

    for (const auto& desc : *entities) {
        const auto entity = desc.ent;
        const auto handle = desc.handle;

        if (entity == nullptr) {
            continue;
        }

        const auto behavior = entity->behavior;

        if (behavior == nullptr) {
            continue;
        }

        const auto ti = utility::rtti::get_type_info(behavior);

        if (ti == nullptr) {
            continue;
        }

        behavior_map[ti->name()].push_back(behavior);
    }

    for (const auto& [name, behaviors] : behavior_map) {
        if (ImGui::TreeNode(name.data())) {
            for (const auto& behavior : behaviors) {
                const auto entity = behavior->get_entity();

                if (entity == nullptr) {
                    continue;
                }

                ImGui::PushID(entity);

                if (ImGui::TreeNode(entity->name)) {
                    ImGui::Text("0x%p", (uintptr_t)behavior);
                    ImGui::Text("Handle: 0x%X", entity->handle);

                    if (behavior->is_behavior_appbase()) {
                        const auto appbase = behavior->as<sdk::BehaviorAppBase>();

                        ImGui::Text("HP: %i/%i", appbase->getHp(), appbase->getHpMax());
                    }

                    if (ImGui::Button("terminate")) {
                        behavior->terminate();
                    }

                    if (behavior->isSuspend() ? ImGui::Button("resume") : ImGui::Button("suspend")) {
                        behavior->setSuspend((int)!behavior->isSuspend());
                    }

                    if (behavior->isTrans() ? ImGui::Button("set invis") : ImGui::Button("set visible")) {
                        if (behavior->isTrans()) {
                            behavior->offTrans();
                        } else {
                            behavior->onTrans();
                        }
                    }

                    ImGui::TreePop();
                }

                ImGui::PopID();
            }

            ImGui::TreePop();
        }
    }
}