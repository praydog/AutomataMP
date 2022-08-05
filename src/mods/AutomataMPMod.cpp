#include <mutex>

#include <windows.h>

#include <spdlog/spdlog.h>

#include <utility/Input.hpp>

#include <sdk/Entity.hpp>
#include <sdk/EntityList.hpp>

#include <sdk/Game.hpp>
#include <sdk/ScriptFunctions.hpp>
#include "AutomataMPMod.hpp"

using namespace std;

std::shared_ptr<AutomataMPMod> AutomataMPMod::get() {
    static std::shared_ptr<AutomataMPMod> instance = std::make_shared<AutomataMPMod>();

    return instance;
}

AutomataMPMod::~AutomataMPMod() {
    if (m_client) {
        m_client->disconnect();
    }
}

std::optional<std::string> AutomataMPMod::on_initialize() try {
    spdlog::info("Entering AutomataMPMod.");

    // Do it later.
    enetpp::global_state::get().initialize();

    spdlog::info("Leaving AutomataMPMod.");

    return Mod::on_initialize();
} catch(std::exception& e) {
    spdlog::error("{}", e.what());
    return e.what();
} catch (...) {
    spdlog::error("Unknown exception");
    return "Unknown exception";
}

bool AutomataMPMod::clientConnect() {
    m_client = make_unique<NierClient>("127.0.0.1");

    if (m_client->isConnected()) {
        return true;
    }
    else {
        m_client.reset();
        return false;
    }
}

void AutomataMPMod::serverStart() {
    m_server = make_unique<NierServer>();
}

void AutomataMPMod::sendPacket(const enet_uint8* data, size_t size) {
    if (m_client) {
        m_client->send_packet(0, data, size, ENET_PACKET_FLAG_RELIABLE);
    }

    if (m_server) {
        m_server->send_packet_to_all_if(0, data, size, ENET_PACKET_FLAG_RELIABLE, [](auto& a) { return true; });
    }
}

void AutomataMPMod::on_draw_ui() {
    if (!ImGui::CollapsingHeader("AutomataMPMod")) {
        return;
    }

    if (m_client) {
        ImGui::Text("State: Client");
    } else {
        ImGui::Text("State: Disconnected");
    }

    /*if (ImGui::Button("Start Server")) {
        serverStart();
    }*/
    
    if (ImGui::InputText("Connect IP", m_ip_connect_input.data(), m_ip_connect_input.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
        m_server = {};
        m_client.reset();

        m_client = make_unique<NierClient>(m_ip_connect_input.data(), m_name_input.data(), m_password_input.data());

        if (!m_client->isConnected()) {
            m_client.reset();
        }
    }

    ImGui::InputText("Name", m_name_input.data(), m_name_input.size());
    ImGui::InputText("Password", m_password_input.data(), m_password_input.size());

    if (m_server) {
        m_server->on_draw_ui();
    }

    if (m_client) {
        m_client->on_draw_ui();
    }
}

void AutomataMPMod::on_frame() {
    if (m_server) {
        // Draw "Server" at 0, 0 with red text.
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(0, 0), ImGui::GetColorU32(ImGuiCol_Text), "Server");
    }

    if (m_client) {
        // Draw "Client" at 0, 0 with green text.
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(0, 0), ImGui::GetColorU32(ImGuiCol_Text), "Client");
    }

    if (!m_server && !m_client) {
        // Draw "Disconnected" at 0, 0 with red text.
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(0, 0), ImGui::GetColorU32(ImGuiCol_Text), "Disconnected");
    }
}

void AutomataMPMod::on_think() {
    if (nier::isLoading() || m_wantsDestroyClient) {
        if (m_client != nullptr) {
            m_client->disconnect();
            m_client.reset();
        }

        m_wantsDestroyClient = false;
        return;
    }

    auto entityList = EntityList::get();

    if (!entityList) {
        return;
    }


    auto player = entityList->getByName("Player");

    if (!player) {
        spdlog::info("Player not found");
        return;
    }
    
    auto partners = entityList->getAllByName("partner");
    auto partner = entityList->getByName("partner");

    if (partner) {
        if (utility::was_key_down(VK_F4)) {
            Vector3f* myPos = Address(player->entity).get(0x50).as<Vector3f*>();

            for (auto i : partners) {
                if (!i->entity) {
                    continue;
                }

                Vector3f* vec = Address(i->entity).get(0x50).as<Vector3f*>();
                *vec = *myPos;
            }
        }
    }
    else {
        /*spdlog::info("Spawning partner");

        auto ent = entityList->spawnEntity("partner", EModel::MODEL_2B, *player->entity->getPosition());

        if (ent) {
            ent->entity->setBuddyHandle(player->handle);
            player->entity->setBuddyHandle(ent->handle);

            ent->entity->setSuspend(false);

            ent->assignAIRoutine("PLAYER");
            ent->assignAIRoutine("player");

            // alternate way of assigning AI/control to the entity easily.
            player->entity->changePlayer();
            player->entity->changePlayer();

            const auto old_flags = ent->entity->getBuddyFlags();
            ent->entity->setBuddyFlags(8);
            ent->entity->setBuddyFromNpc();
            ent->entity->setBuddyFlags(old_flags);

            m_players[1].setStartTick(*ent->entity->getTickCount());
        }*/
    }

    //spdlog::info("Player: 0x%p, handle: 0x%X", player, player->handle);
    //spdlog::info("Partner: 0x%p, handle: 0x%X", partner, partner->handle);
    //spdlog::info(" partner real ent: 0x%p", partner->entity);

    static uint32_t(*possessEntity)(Entity* player, uint32_t* handle, bool a3) = (decltype(possessEntity))0x1402118D0;
    static uint32_t(*unpossessEntity)(Entity* player, bool a2) = (decltype(unpossessEntity))0x140211AE0;

    /*if (utility::was_key_down(VK_F5)) {
        auto curHandle = Address(0x1416053E0).as<uint32_t*>();
        auto curEnt = entityList->getByHandle(*curHandle);

        if (!curEnt)
            return;

        auto pl = entityList->getAllByName("Player");
        auto players = entityList->getAllByName("partner");
        players.insert(players.end(), pl.begin(), pl.end());

        auto curPlayer = players.begin();

        for (auto& i : *entityList) {
            if (!i.ent || !i.handle || i.handle == player->handle || i.handle == *curHandle || std::find(players.begin(), players.end(), i.ent) != players.end())
                continue;

            if (!i.ent->entity || i.ent->handle == *curHandle)
                continue;

            if (i.ent->entity->getHealth() == 0)
                continue;

            if ((*curPlayer)->entity->getBuddyThing() == 0x10200) {
                if ((*curPlayer)->entity->getPossessedHandle() != 0) {
                    unpossessEntity((*curPlayer)->entity, true);
                }

                possessEntity((*curPlayer)->entity, &i.handle, true);

                if ((*curPlayer)->entity->getPossessedHandle() != 0) {
                    curPlayer++;
                }
            }
            else
                curPlayer++;

            if (curPlayer == players.end())
                break;
        }
    }*/

    if (utility::was_key_down(VK_F6)) {
        player->entity->changePlayer();
        //nier_client_and_server::ChangePlayer change;
        //sendPacket(change.data(), sizeof(change));
    }

    if (utility::was_key_down(VK_F7)) {
        for (auto& i : *entityList) {
            if (!i.ent || !i.handle)
                continue;

            if (!i.ent->entity)
                continue;

            if (i.ent->entity->getHealth() == 0)
                continue;

            i.ent->entity->setBuddyFlags(-1);
            i.ent->entity->setBuddyFromNpc();
            i.ent->entity->setBuddyFlags(8);
            i.ent->entity->setBuddyFromNpc();
            i.ent->entity->setBuddyFlags(1);
        }
    }

    auto prevPlayer = player;

    // generates a linked list of players pretty much
    // so we can swap between all of them instead of just two.
    for (uint32_t index = 0; index < entityList->size(); ++index) {
        auto ent = entityList->get(index);

        if (!ent || !ent->entity) {
            continue;
        }
        
        if (ent->name != string("Player") && ent->name != string("partner"))
            continue;

        if (prevPlayer == ent)
            continue;

        prevPlayer->entity->setBuddyHandle(ent->handle);
        prevPlayer = ent;
    }

    if (prevPlayer != player) {
        prevPlayer->entity->setBuddyHandle(player->handle);
    }

    static uint32_t(*spawnBuddy)(Entity* player) = (decltype(spawnBuddy))0x140245C30;

    sharedThink();

    if (utility::was_key_down(VK_F9)) {
        /*auto old = player->entity->getBuddyHandle();
        player->entity->setBuddyHandle(0);
        spawnBuddy(player->entity);
        player->entity->setBuddyHandle(old);*/

        auto ent = entityList->spawnEntity("partner", EModel::MODEL_2B, *player->entity->getPosition());

        if (ent) {
            ent->assignAIRoutine("buddy_2B");
            ent->assignAIRoutine("buddy");

            ent->entity->setBuddyHandle(player->handle);
            player->entity->setBuddyHandle(ent->handle);

            // alternate way of assigning AI to the entity easily.
            //changePlayer(player->entity);
            //changePlayer(player->entity);

            ent->entity->setSuspend(false);

            ent->entity->setBuddyFlags(-1);
            ent->entity->setBuddyFromNpc();
            ent->entity->setBuddyFlags(1);

            //ent->entity->setPosRotResetHap(Vector4f{*player->entity->getPosition(), 1.0f}, glm::identity<glm::quat>());
        }

        spdlog::info("{:x}", (uintptr_t)ent);
    }

    if (utility::was_key_down(VK_F10) && partner) {
        for (auto p : partners) {
            p->entity->terminate();
        }
    }

    /*if (utility::was_key_down(VK_F2)) {
        Entity::Signal signal;
        signal.signal = 0xEB1B2287;
        player->entity->signal(signal);
    }*/

    if (utility::was_key_down(VK_F3)) {
        player->entity->setSuspend(!player->entity->isSuspend());
    }
}

void AutomataMPMod::sharedThink() {
    //spdlog::info("Shared think");

    //static uint32_t(*changePlayer)(Entity* player) = (decltype(changePlayer))0x1401ED500;

    auto entityList = EntityList::get();

    if (!entityList) {
        return;
    }

    // main player entity that game is originally controlling
    auto player = entityList->getByName("Player");

    if (!player) {
        spdlog::info("Player not found");
        return;
    }

    auto controlledEntity = entityList->getPossessedEntity();

    if (!controlledEntity || !controlledEntity->entity) {
        spdlog::info("Controlled entity invalid");
        return;
    }

    /*if (m_client && controlledEntity->name != string("partner")) {
        auto realBuddy = entityList->getByHandle(controlledEntity->entity->getBuddyHandle());

        if (realBuddy && realBuddy->entity) {
            //realBuddy->entity->setBuddyFlags(0);
            realBuddy->entity->setSuspend(false);
            //changePlayer(player->entity);
            player->entity->changePlayer();
        }

        return;
    }*/

    m_midHooks.addOverridenEntity(controlledEntity->entity);
    m_playerHook.reHook(controlledEntity->entity);
    controlledEntity->entity->setBuddyFlags(0);

    auto realBuddy = entityList->getByHandle(controlledEntity->entity->getBuddyHandle());

    m_networkEntities.think();

    if (m_client) {
        m_client->think();
    }
}

void AutomataMPMod::synchronize() {
    if (m_client == nullptr) {
        return;
    }

    for (auto& it : m_client->getPlayers()) {
        const auto& networkedPlayer = it.second;

        // Do not update the local player here.
        if (networkedPlayer == nullptr || networkedPlayer->getGuid() == m_client->getGuid()) {
            continue;
        }

        auto npc = networkedPlayer->getEntity();

        if (npc == nullptr) {
            spdlog::error("NPC for player {} not found", networkedPlayer->getGuid());
            continue;
        }

        spdlog::info("Synchronizing player {}", networkedPlayer->getGuid());

        auto& data = networkedPlayer->getPlayerData();
        *npc->getRunSpeedType() = SPEED_PLAYER;
        *npc->getFlashlightEnabled() = data.flashlight();
        *npc->getSpeed() = data.speed();
        *npc->getFacing() = data.facing();
        *npc->getFacing2() = data.facing2();
        *npc->getWeaponIndex() = data.weapon_index();
        *npc->getPodIndex() = data.pod_index();
        npc->getCharacterController()->heldFlags = data.held_button_flags();
        //*npc->getPosition() = movement.position;
    }
}

void AutomataMPMod::serverPacketProcess(const Packet* data, size_t size) {
    spdlog::info("Server packet {} received", data->id);

    switch (data->id) {
    case ID_SPAWN_ENTITY:
        processEntitySpawn((nier_server::EntitySpawn*)data);
        break;
    case ID_ENTITY_DATA:
        processEntityData((nier_server::EntityData*)data);
        break;
    default:
        break;
    }
}

void AutomataMPMod::sharedPacketProcess(const Packet* data, size_t size) {
}

void AutomataMPMod::processPlayerData(const nier_client_and_server::PlayerData* movement) {
}

void AutomataMPMod::processAnimationStart(const nier_client_and_server::AnimationStart* animation) {
}

void AutomataMPMod::processButtons(const nier_client_and_server::Buttons* buttons) {
}

void AutomataMPMod::processEntitySpawn(nier_server::EntitySpawn* spawn) {
    /*spdlog::info("Enemy spawn received");
    auto entityList = EntityList::get();

    if (entityList) {
        EntitySpawnParams params;
        EntitySpawnParams::PositionalData matrix = spawn->matrix;
        params.matrix = &matrix;
        params.model = spawn->model;
        params.model2 = spawn->model2;
        params.name = spawn->name;

        spdlog::info("Spawning {}", params.name);

        auto ent = entityList->spawnEntity(params);

        if (ent) {
            m_networkEntities.addEntity(ent, spawn->guid);
        }
    }*/
}

void AutomataMPMod::processEntityData(nier_server::EntityData* data) {
    m_networkEntities.processEntityData(data);
}
