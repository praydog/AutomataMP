#include <unordered_map>
#include <map>
#include <functional>
#include <fstream>
#include <filesystem>

#include <imgui.h>
#include <utility/RTTI.hpp>

#include <sdk/Behavior.hpp>
#include <sdk/EntityList.hpp>
#include <sdk/Entity.hpp>
#include <sdk/NPC.hpp>
#include <sdk/EmBase.hpp>
#include <sdk/Pl0000.hpp>
#include <sdk/hap/TokenCategory.hpp>
#include <sdk/hap/scene_state/SceneStateSystem.hpp>
#include <sdk/mruby.hpp>

#include "Explorer.hpp"

struct Method {
    std::function<void ()> method;
    const char* name;
};

auto disp_same_line_noargs = [](const std::vector<Method>& methods) {
    for (auto it = methods.begin(); it != methods.end(); ++it) {
        if (ImGui::Button(it->name)) {
            it->method();
        }

        if (it + 1 != methods.end()) {
            ImGui::SameLine();
        }
    }
};

void Explorer::on_draw_ui() {
    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    auto mrb = sdk::mruby::get_state();
    ImGui::Text("mruby state: %p", (void*)mrb);

    if (mrb != nullptr) {
        if (ImGui::Button("Inject custom methods")) {
            mrb_define_method(mrb, mrb->object_class, "really_cool_method", [](mrb_state* mrb, mrb_value self) {
                MessageBox(nullptr, "Hello from mruby!", "Hello", MB_OK);
                return mrb_nil_value();
            }, MRB_ARGS_NONE());

            // dump a string to disk
            mrb_define_method(mrb, mrb->object_class, "dump_to_disk", [](mrb_state* mrb, mrb_value self) {
                mrb_value str;
                mrb_get_args(mrb, "S", &str);

                auto path = std::filesystem::current_path() / "dump.txt";
                std::ofstream ofs(path);
                ofs << mrb_str_to_cstr(mrb, str);
                ofs.close();

                return mrb_nil_value();
            }, MRB_ARGS_REQ(1));
        }

        char eval_buffer[256] = { 0 };
        static mrb_value last_result{};

        if (ImGui::InputTextMultiline("eval", eval_buffer, sizeof(eval_buffer), ImVec2{}, ImGuiInputTextFlags_EnterReturnsTrue)) {
            last_result = mrb_obj_as_string(sdk::mruby::get_state(), sdk::mruby::eval(eval_buffer));
        }

        if (last_result.tt == MRB_TT_STRING) {
            ImGui::TextWrapped("result: %s", mrb_string_value_cstr(mrb, &last_result));
        } else {
            ImGui::TextWrapped("result: %p", last_result.value.p);
        }
    }

    ImGui::Text("first token: %p", (void*)sdk::hap::TokenCategory::get_first());
    ImGui::Text("Scene state: %p", (void*)sdk::hap::scene_state::SceneStateSystem::get());

    display_player_options();

    if (ImGui::TreeNode("Entities")) {
        display_entities();
        ImGui::TreePop();
    }
}

void Explorer::display_player_options() {
    const auto entities = sdk::EntityList::get();

    if (entities == nullptr) {
        return;
    }

    if (ImGui::TreeNode("Player")) {
        const auto player_ent = entities->get_by_name("Player");
        const auto player_behavior = player_ent != nullptr ? player_ent->behavior->try_cast<sdk::Pl0000>() : nullptr;

        if (player_behavior != nullptr) {
            display_pl0000(player_behavior);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Controlled Entity")) {
        const auto controlled = entities->get_possessed_entity();

        if (controlled != nullptr) {
            display_behavior(controlled->behavior);
        }
        else {
            ImGui::Text("No controlled entity.");
        }
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

    const auto player_ent = entities->get_by_name("Player");
    const auto player_behavior = player_ent != nullptr ? player_ent->behavior->try_cast<sdk::Pl0000>() : nullptr;

    for (const auto& [name, behaviors] : behavior_map) {
        if (ImGui::TreeNode(name.data())) {
            for (const auto& behavior : behaviors) {
                const auto entity = behavior->get_entity();

                if (entity == nullptr) {
                    continue;
                }

                ImGui::PushID(entity);

                if (ImGui::TreeNode(entity->name)) {
                    display_behavior(behavior, player_behavior);

                    ImGui::TreePop();
                }

                ImGui::PopID();
            }

            ImGui::TreePop();
        }
    }
}

void Explorer::display_behavior(sdk::Behavior* behavior, sdk::Behavior* player_behavior) {
    const auto entity = behavior->get_entity();

    if (entity == nullptr) {
        return;
    }

    ImGui::PushID(entity);

    ImGui::Text("0x%p", (uintptr_t)behavior);
    ImGui::Text("Handle: 0x%X", entity->handle);

    if (const auto appbase = behavior->try_cast<sdk::BehaviorAppBase>(); appbase != nullptr) {
        ImGui::Text("HP: %i/%i", appbase->getHp(), appbase->getHpMax());
    }

    if (const auto pl0000 = behavior->try_cast<sdk::Pl0000>(); pl0000 != nullptr) {
        display_pl0000(pl0000);
    }

    if (const auto em = behavior->try_cast<sdk::EmBase>(); em != nullptr) {
        display_em_base(em, player_behavior);
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

    ImGui::PopID();
}

void Explorer::display_em_base(sdk::EmBase* em, sdk::Behavior* player_behavior) {
    disp_same_line_noargs({
        { [&]() { em->callEnemy(); }, "call enemy" }
    });

    disp_same_line_noargs({
        { [&]() { em->changeEnemy(); }, "change enemy" },
        { [&]() { em->changeNPC(); }, "change NPC" },
    });

    disp_same_line_noargs({
        { [&]() { em->setNpcScare(); }, "set scared" },
    });


    if (player_behavior != nullptr) {
        if (ImGui::Button("move to player")) {
            em->setGoPoint(Vector4f{player_behavior->position(), 1.0f});
        }
    }
}

void Explorer::display_pl0000(sdk::Pl0000* pl0000) {
    if (ImGui::Button("destroy buddy")) {
        pl0000->destroyBuddy();
    }

    auto disp_same_line_noargs = [&](const std::vector<Method>& methods) {
        for (auto it = methods.begin(); it != methods.end(); ++it) {
            if (ImGui::Button(it->name)) {
                it->method();
            }

            if (it + 1 != methods.end()) {
                ImGui::SameLine();
            }
        }
    };

    disp_same_line_noargs({
        { [&]() { pl0000->enableFriendlyFire(); }, "enable friendly fire" },
        { [&]() { pl0000->disableFriendlyFire(); }, "disable friendly fire" },
    });

    disp_same_line_noargs({
        { [&]() { pl0000->enableFriendlyFireYorha(); }, "enable friendly fire yorha" },
        { [&]() { pl0000->disableFriendlyFireYorha(); }, "disable friendly fire yorha" },
    });

    disp_same_line_noargs({
        { [&]() { pl0000->changePlayer(); }, "change player" },
        { [&]() { pl0000->changePlayerFinal(); }, "change player final" },
        { [&]() { pl0000->changePlayerFinalA2(); }, "change player final A2" },
    });

    disp_same_line_noargs({
        { [&]() { pl0000->addRedGirl(); }, "add red girl" },
        { [&]() { pl0000->lostRedGirl(); }, "lost red girl" },
    });

    disp_same_line_noargs({
        { [&]() { pl0000->set2BBreak(); }, "2B break" },
        { [&]() { pl0000->set9sBreak(); }, "9S break" },
        { [&]() { pl0000->setA2Break(); }, "A2 break" },
    });

    disp_same_line_noargs({
        { [&]() { pl0000->set2BBreakWeak(); }, "2B break weak" },
        { [&]() { pl0000->set9sBreakWeak(); }, "9S break weak" },
        { [&]() { pl0000->setA2BreakWeak(); }, "A2 break weak" },
    });

    disp_same_line_noargs({
        { [&]() { pl0000->end2BBreak(); }, "end 2B break" },
        { [&]() { pl0000->end9sBreak(); }, "end 9S break" },
        { [&]() { pl0000->endA2Break(); }, "end A2 break" },
    });

    disp_same_line_noargs({
        { [&]() { pl0000->setBedSit(); }, "bed sit" },
        { [&]() { pl0000->setBedStandup(); }, "bed standup" },
    });

    disp_same_line_noargs({
        { [&]() { pl0000->start2BMoveTo9S(); }, "2B move to 9S" },
        { [&]() { pl0000->start9sMoveTo2B(); }, "9S move to 2B" },
    });

    disp_same_line_noargs({
        { [&]() { pl0000->setPlayerFromNPC(); }, "set player from NPC" },
        { [&]() { pl0000->setBuddyFromNpc(); }, "set buddy from NPC" },
        { [&]() { pl0000->setNpcFromBuddy(); }, "set NPC from buddy" },
    });
}