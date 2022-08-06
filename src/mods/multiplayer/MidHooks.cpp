#include <string>

#include <spdlog/spdlog.h>

#include <SafetyHook.hpp>
#include <asmjit/asmjit.h>

#include <sdk/Entity.hpp>
#include <sdk/EntityList.hpp>

#include <utility/Scan.hpp>
#include <utility/Module.hpp>
#include <utility/Input.hpp>

#include "Mods.hpp"
#include "mods/AutomataMPMod.hpp"
#include "AutomataMP.hpp"

#include "MidHooks.hpp"

using namespace std;

MidHooks* g_MidHooks = nullptr;

uintptr_t get_on_update_function() {
    static uintptr_t on_update_function = []() -> uintptr_t {
        spdlog::info("[MidHooks] Finding on_update_function...");

        const auto middle = utility::scan(utility::get_executable(), "? 89 ? e8 00 00 00 ? 89 ? f0 00 00 00 ? ? ? e8 ? ? ? ? ? ? ? ? ? 00 00");

        if (!middle) {
            spdlog::error("[MidHooks] Failed to find on_update_function.");
            return 0;
        }

        const auto int3s = utility::scan_reverse(*middle, 0x100, "CC CC CC");

        if (!int3s) {
            spdlog::error("[MidHooks] Failed to find int3s.");
            return 0;
        }

        const auto result = *int3s + 3;

        spdlog::info("[MidHooks] on_update_function: {:x}", result);

        return result;
    }();

    return on_update_function;
}

// gets an instruction near the end of the function that has an easy-ish to scan for pattern
uintptr_t get_on_processed_buttons() {
    static uintptr_t on_processed_buttons = []() -> uintptr_t {
        spdlog::info("[MidHooks] Finding on_processed_buttons...");

        const auto middle = utility::scan(utility::get_executable(), "C7 ? 90 07 00 00 14 00 00 00 48");

        if (!middle) {
            spdlog::error("[MidHooks] Failed to find on_processed_buttons.");
            return 0;
        }

        const auto result = *middle + 10;

        spdlog::info("[MidHooks] on_processed_buttons: {:x}", result);

        return result;
    }();

    return on_processed_buttons;
}

// near the top of ProcessButtons
uintptr_t get_on_set_held_flags() {
    static uintptr_t pre_process_buttons = []() -> uintptr_t {
        spdlog::info("[MidHooks] Finding on_set_held_flags...");

        const auto middle = get_on_processed_buttons();
        const auto ref = utility::scan_reverse(middle, 0x1000, "CC CC CC");

        if (!ref) {
            spdlog::error("[MidHooks] Failed to find pre_process_buttons.");
            return 0;
        }

        const auto func = *ref + 3;

        spdlog::info("[MidHooks] pre_process_buttons: {:x}", func);

        const auto result = utility::scan_disasm(func, 0x100, "48 89 81 40 07 00 00");

        if (!result) {
            spdlog::error("[MidHooks] Failed to find on_set_held_Flags.");
            return 0;
        }

        spdlog::info("[MidHooks] on_set_held_flags: {:x}", *result);

        return *result;
    }();

    return pre_process_buttons;
}

uintptr_t get_entity_terminate_fn() {
    static uintptr_t entity_terminate_fn = []() -> uintptr_t {
        spdlog::info("[MidHooks] Finding entity_terminate_fn...");

        const auto terminate = ScriptFunctions::get().find("Behavior.terminate");

        if (!terminate) {
            spdlog::error("[MidHooks] Failed to find entity_terminate_fn.");
            return 0;
        }

        const auto terminate_script_fn = terminate->function;

        spdlog::info("[MidHooks] entity_terminate_script_fn: {:x}", terminate_script_fn);

        const auto ref = utility::scan_disasm(terminate_script_fn, 0x100, "0F 85");

        if (!ref) {
            spdlog::error("[MidHooks] Failed to find entity_terminate_fn.");
            return 0;
        }

        const auto result = utility::calculate_absolute(*ref + 2);

        spdlog::info("[MidHooks] entity_terminate_fn: {:x}", result);

        return result;
    }();

    return entity_terminate_fn;
}

MidHooks::MidHooks() {
    spdlog::info("[MidHooks] Initializing...");

    g_MidHooks = this;

    // required to allow button to be held down without resetting
    /*m_hook.hook(0x140262B22, [=](const VehHook::RuntimeInfo& info) {
        auto entity = Address(info.context->Rcx).get(-0xCA0).as<Entity*>();

        auto& player = AutomataMPMod::get()->getPlayers()[1];

        if (entity == player.getEntity()) {
            entity->getCharacterController()->heldFlags = player.getPlayerData().heldButtonFlags;
        }
    });*/

    // Early version hooks
    /*addHook(0x140263006, &MidHooks::onProcessedButtons);
    addHook(0x1404F9AA0, &MidHooks::onPreEntitySpawn);
    addHook(0x1404F9BE9, &MidHooks::onPostEntitySpawn);
    addHook(0x1404F8DE0, &MidHooks::onEntityTerminate);
    addHook(0x140519460, &MidHooks::onUpdate);*/

    /*addHook(0x140263006, &MidHooks::onProcessedButtons);
    addHook(0x1404F9AA0, &MidHooks::onPreEntitySpawn);
    addHook(0x1404F9BE9, &MidHooks::onPostEntitySpawn);
    addHook(0x1404F8DE0, &MidHooks::onEntityTerminate);*/

    const auto [spawn_fn, spawn_this] = EntityList::getSpawnEntityFn();
    addHook((uintptr_t)spawn_fn, &MidHooks::onPreEntitySpawn);
    addHook((uintptr_t)EntityList::getPostSpawnEntityFn(), &MidHooks::onPostEntitySpawn);
    addHook(get_entity_terminate_fn(), &MidHooks::onEntityTerminate);
    // todo: hook the other version of the terminate function (the script function)
    addHook(get_on_update_function(), &MidHooks::onUpdate);
    addHook(get_on_processed_buttons(), &MidHooks::onProcessedButtons);

    addHook(get_on_set_held_flags(), [](safetyhook::Context& context) {
        auto entity = Address(context.rcx).get(-0xCA0).as<Entity*>();

        const auto& amp = AutomataMPMod::get();
        const auto& client = amp->getClient();

        if (client == nullptr) {
            return;
        }

        const auto& players = client->getPlayers();

        auto it = std::find_if(players.begin(), players.end(), [&](auto& player) {
            return player.second->getEntity() == entity;
        });

        if (it == players.end() || it->second == nullptr) {
            return;
        }

        if (entity == it->second->getEntity()) {
            entity->getCharacterController()->heldFlags = it->second->getPlayerData().held_button_flags();
        }
    });

    spdlog::info("[MidHooks] Initialized.");
}

void MidHooks::onLightAttack(const VehHook::RuntimeInfo& info) {
    auto entity = Address(info.context->R8).get(-0xCA0).as<Entity*>();

    if (m_overridenEntities.count(entity) != 0) {
        nier_client_and_server::Buttons buttons;
        buttons.buttons[Entity::CharacterController::INDEX_ATTACK_LIGHT] = (uint32_t)info.context->Rax;
        buttons.buttons[Entity::CharacterController::INDEX_ATTACK_HEAVY] = (uint32_t)info.context->Rdx;
        AutomataMPMod::get()->sendPacket(buttons.data(), sizeof(buttons));
    }
}

void MidHooks::onHeavyAttack(const VehHook::RuntimeInfo & info) {
    auto entity = Address(info.context->R8).get(-0xCA0).as<Entity*>();

    if (m_overridenEntities.count(entity) != 0) {
        nier_client_and_server::Buttons buttons;
        buttons.buttons[Entity::CharacterController::INDEX_ATTACK_LIGHT] = (uint32_t)info.context->Rdx;
        buttons.buttons[Entity::CharacterController::INDEX_ATTACK_HEAVY] = (uint32_t)info.context->Rax;
        AutomataMPMod::get()->sendPacket(buttons.data(), sizeof(buttons));
    }
}

void MidHooks::onProcessedButtons(safetyhook::Context& context)
{   
    // OLD VERSION USES R8. THE REGISTER IS THE CHARACTER CONTROLLER OF THE ENTITY.
    //auto entity = Address(info.context->R8).get(-0xCA0).as<Entity*>();
    auto entity = Address(context.rbx).get(-0xCA0).as<Entity*>();

    if (m_overridenEntities.count(entity) != 0) {
        const auto amp = AutomataMPMod::get();
        auto& client = amp->getClient();

        if (client == nullptr) {
            return;
        }

        for (auto button : entity->getCharacterController()->buttons) {
            if (button != 0) {
                client->sendButtons(entity->getCharacterController()->buttons);
                break;
            }
        }
    }
}

void MidHooks::onPreEntitySpawn(safetyhook::Context& context) {
    scoped_lock<mutex> _(m_spawnMutex);

    if (!AutomataMPMod::get()->isServer()) {
        return;
    }

    auto spawnParams = (EntitySpawnParams*)context.rdx;

    if (spawnParams->name) {
        if (spawnParams->name != string("FreeEnemy")) {
            return;
        }

        m_threadIdToSpawnParams[GetCurrentThreadId()] = spawnParams;
    }
}

void MidHooks::onPostEntitySpawn(safetyhook::Context& context) {
    scoped_lock<mutex> _(m_spawnMutex);

    auto threadId = GetCurrentThreadId();

    if (m_threadIdToSpawnParams.find(threadId) == m_threadIdToSpawnParams.end()) {
        return;
    }

    auto entity = (EntityContainer*)context.rax;

    if (entity) {
        AutomataMPMod::get()->onEntityCreated(entity, m_threadIdToSpawnParams[threadId]);
    }

    m_threadIdToSpawnParams.erase(threadId);
}

void MidHooks::onEntityTerminate(safetyhook::Context& context) {
    auto ent = (EntityContainer*)context.rcx;

    AutomataMPMod::get()->onEntityDeleted(ent);
}

// Potential OnUpdate candidates
/*
Function calls:
0x0000000140519460: 1
0x000000014051DCF0: 1
0x00000001401A35C0: 5
0x00000001405B6AC0: 5
0x00000001401A34F0: 5
0x00000001401A3440: 5
0x00000001401A3640: 5
0x00000001401A36C0: 5
0x00000001408E6730: 5
0x0000000140519470: 1
0x0000000140001060: 1
0x00000001408E6A10: 5
0x00000001408134B0: 5
0x00000001408082C0: 5
0x0000000140813520: 5
0x00000001408710A0: 5
0x00000001408710F0: 5
0x00000001408711A0: 5
0x00000001408E6900: 5
0x00000001408135F0: 5
0x0000000140938030: 5
*/

void MidHooks::onUpdate(safetyhook::Context& info) {
    auto entityList = EntityList::get();
    auto player = entityList->getByName("Player");

    if (!player) {
        return;
    }

    auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        mod->on_think();
    }

    if (utility::was_key_down(VK_F1) && AutomataMPMod::get()->isServer()) {
        /*static std::unordered_map<uint32_t, uint32_t> models{ { EModel::MODEL_2B, EModel::MODEL_9S }, 
                                                              { EModel::MODEL_9S, EModel::MODEL_A2 }, 
                                                              { EModel::MODEL_A2, EModel::MODEL_2B }};

        auto ent = entityList->spawnEntity("Player", models[*player->entity->getModel()], *player->entity->getPosition());

        if (ent) {
            player->entity->setBuddyHandle(ent->handle);
            ent->entity->setSuspend(false);
            player->entity->changePlayer();
        }*/

        EntitySpawnParams params;
        EntitySpawnParams::PositionalData matrix;
        matrix.position = Vector4f{*player->entity->getPosition(), 1.0f};
        matrix.unknown = *Address(player->entity).get(0x90).as<Vector4f*>();
        params.name = "FreeEnemy";
        params.matrix = &matrix;
        params.model = 0x21020;
        params.model2 = 0x21020;

        auto test = entityList->spawnEntity(params);

        if (test) {
            test->entity->setSuspend(false);
            AutomataMPMod::get()->getClient()->onEntityCreated(test, &params);
        }

        return;
    }

    if (utility::was_key_down(VK_F2)) {
        auto enemies = entityList->getAllByName("FreeEnemy");

        for (auto i : enemies) {
            if (!i->entity) {
                continue;
            }

            i->entity->terminate();
        }
    }
}

void MidHooks::addOverridenEntity(Entity* ent) {
    m_overridenEntities.insert(ent);
}

void MidHooks::addHook(uintptr_t address, MemberMidCallbackFn cb) {
    std::scoped_lock _{m_hookMutex};

    auto factory = SafetyHookFactory::init();
    auto builder = factory->acquire();

    using namespace asmjit;
    using namespace asmjit::x86;

    //std::scoped_lock _{m_jit_mux};
    CodeHolder code{};
    code.init(m_jit.environment());

    Assembler a{&code};

    const void* realCb = (void*&)cb;

    a.mov(rdx, rcx);
    a.mov(rcx, g_MidHooks);
    a.mov(rax, realCb);
    a.jmp(rax);

    uintptr_t code_addr{};
    m_jit.add(&code_addr, &code);

    m_midHooks.emplace_back(builder.create_mid((void*)address, (safetyhook::MidHookFn)code_addr));
}

void MidHooks::addHook(uintptr_t address, safetyhook::MidHookFn cb) {
    std::scoped_lock _{m_hookMutex};

    auto factory = SafetyHookFactory::init();
    auto builder = factory->acquire();

    m_midHooks.emplace_back(builder.create_mid((void*)address, cb));
}