#include <mutex>

#include <windows.h>

#include <spdlog/spdlog.h>

#include <utility/Input.hpp>
#include <utility/HttpClient.hpp>
#include <json.hpp>

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

    std::strcpy(m_master_server_input.data(), "https://niermaster.praydog.com");

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

void AutomataMPMod::display_servers() {
    const auto now = std::chrono::steady_clock::now();

    // Check if server future is ready and parse it into our internal server list.
    if (m_server_future.valid() && m_server_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        m_servers.clear();

        const auto response = m_server_future.get();
        spdlog::info("Got response: {}", response);
        const auto response_json = nlohmann::json::parse(response);

        try {
            for (const auto& [ip, jsondata] : response_json.items()) {
                auto new_server_data = std::make_unique<AutomataMPMod::ServerData>();

                const auto data = jsondata["Data"];

                new_server_data->ip = ip;
                new_server_data->name = data["Name"];
                new_server_data->num_players = data["num_players"];

                m_servers.push_back(std::move(new_server_data));
            }
        } catch (const std::exception& e) {
            spdlog::error("Error parsing server response: {}", e.what());
        } catch (...) {
            spdlog::error("Unknown Error parsing server response");
        }
    }

    // Render the actual server list.
    for (auto& server : m_servers) {
        ImGui::PushID(server->ip.c_str());

        if (ImGui::Button("Connect")) {
            m_client.reset();
            m_client = make_unique<NierClient>(server->ip.data(), m_name_input.data(), m_password_input.data());

            if (!m_client->is_connected()) {
                m_client.reset();
            }

            return;
        }

        ImGui::SameLine();

        const auto made = ImGui::TreeNode((server->name + " (" + std::to_string(server->num_players) + " players)").c_str());

        if (made) {
            ImGui::Text("IP: %s", server->ip.c_str());
            ImGui::Text("Name: %s", server->name.c_str());
            ImGui::Text("Players: %s", std::to_string(server->num_players).c_str());
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    if (now - m_last_server_update < std::chrono::seconds(5)) {
        return;
    }

    if (m_server_future.valid()) {
        if (m_server_future.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
            return;
        }

        m_server_future.wait();
    }

    m_server_future = std::async(std::launch::async, [this]() -> std::string {
        try {
            HttpClient http{};

            const auto servers_url = std::string{m_master_server_input.data()} + "/servers";
            http.get(servers_url, "", "");

            const auto response = http.response();     
            m_last_server_update = std::chrono::steady_clock::now();

            return response;
        } catch (...) {
            m_last_server_update = std::chrono::steady_clock::now();
            return "";
        }
    });
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

    if (ImGui::InputText("Connect IP", m_ip_connect_input.data(), m_ip_connect_input.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
        m_client.reset();

        m_client = make_unique<NierClient>(m_ip_connect_input.data(), m_name_input.data(), m_password_input.data());

        if (!m_client->is_connected()) {
            m_client.reset();
        }
    }

    ImGui::InputText("Name", m_name_input.data(), m_name_input.size());
    ImGui::InputText("Password", m_password_input.data(), m_password_input.size());

    ImGui::InputText("Master Server", m_master_server_input.data(), m_master_server_input.size());
    if (ImGui::TreeNode("Servers")) {
        display_servers();
        ImGui::TreePop();
    }
    
    if (m_client) {
        m_client->on_draw_ui();
    }
}

void AutomataMPMod::on_frame() {
    if (m_client && m_client->is_master_client()) {
        // Draw "Server" at 0, 0 with red text.
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(0, 0), ImGui::GetColorU32(ImGuiCol_Text), "MasterClient");
    }
    else if (m_client) {
        // Draw "Client" at 0, 0 with green text.
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(0, 0), ImGui::GetColorU32(ImGuiCol_Text), "Client");
    }

    if (!m_client) {
        // Draw "Disconnected" at 0, 0 with red text.
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(0, 0), ImGui::GetColorU32(ImGuiCol_Text), "Disconnected");
    }

    if (m_client) {
        m_client->on_frame();
    }
}

void AutomataMPMod::on_think() {
    if (sdk::is_loading() || m_wants_destroy_client) {
        if (m_client != nullptr) {
            m_client->disconnect();
            m_client.reset();
        }

        m_wants_destroy_client = false;
        return;
    }

    auto entityList = sdk::EntityList::get();

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
            Vector3f* myPos = Address(player->behavior).get(0x50).as<Vector3f*>();

            for (auto i : partners) {
                if (!i->behavior) {
                    continue;
                }

                Vector3f* vec = Address(i->behavior).get(0x50).as<Vector3f*>();
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

    //static uint32_t(*possessEntity)(Entity* player, uint32_t* handle, bool a3) = (decltype(possessEntity))0x1402118D0;
    //static uint32_t(*unpossessEntity)(Entity* player, bool a2) = (decltype(unpossessEntity))0x140211AE0;

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
        player->behavior->as<sdk::Pl0000>()->changePlayer();
        //nier_client_and_server::ChangePlayer change;
        //sendPacket(change.data(), sizeof(change));
    }

    if (utility::was_key_down(VK_F7)) {
        for (auto& i : *entityList) {
            if (!i.ent || !i.handle)
                continue;

            if (!i.ent->behavior)
                continue;

            if (i.ent->behavior->as<sdk::BehaviorAppBase>()->health() == 0){
                continue;
            }

            auto pl0000 = i.ent->behavior->as<sdk::Pl0000>();

            pl0000->obj_flags() = -1;
            pl0000->setBuddyFromNpc();
            pl0000->obj_flags() = 8;
            pl0000->setBuddyFromNpc();
            pl0000->obj_flags() = 1;
        }
    }

    /*auto prevPlayer = player;

    // generates a linked list of players pretty much
    // so we can swap between all of them instead of just two.
    for (uint32_t index = 0; index < entityList->size(); ++index) {
        auto ent = entityList->get(index);

        if (!ent || !ent->behavior) {
            continue;
        }
        
        if (ent->name != string("Player") && ent->name != string("partner"))
            continue;

        if (prevPlayer == ent)
            continue;

        prevPlayer->behavior->as<sdk::Pl0000>()->setBuddyHandle(ent->handle);
        prevPlayer = ent;
    }

    if (prevPlayer != player) {
        prevPlayer->behavior->as<sdk::Pl0000>()->setBuddyHandle(player->handle);
    }*/

    //static uint32_t(*spawnBuddy)(Entity* player) = (decltype(spawnBuddy))0x140245C30;

    shared_think();

    if (utility::was_key_down(VK_F9)) {
        /*auto old = player->entity->getBuddyHandle();
        player->entity->setBuddyHandle(0);
        spawnBuddy(player->entity);
        player->entity->setBuddyHandle(old);*/

        auto ent = entityList->spawnEntity("partner", sdk::EModel::MODEL_2B, player->behavior->position());

        if (ent) {
            ent->assignAIRoutine("buddy_2B");
            ent->assignAIRoutine("buddy");

            ent->behavior->as<sdk::Pl0000>()->buddy_handle() = player->handle;
            player->behavior->as<sdk::Pl0000>()->buddy_handle() = ent->handle;

            // alternate way of assigning AI to the entity easily.
            //changePlayer(player->entity);
            //changePlayer(player->entity);

            ent->behavior->setSuspend(false);

            ent->behavior->obj_flags() = -1;
            ent->behavior->as<sdk::Pl0000>()->setBuddyFromNpc();
            ent->behavior->obj_flags() = 1;

            //ent->entity->setPosRotResetHap(Vector4f{*player->entity->getPosition(), 1.0f}, glm::identity<glm::quat>());
        }

        spdlog::info("{:x}", (uintptr_t)ent);
    }

    if (utility::was_key_down(VK_F10) && partner) {
        for (auto p : partners) {
            p->behavior->terminate();
        }
    }

    /*if (utility::was_key_down(VK_F2)) {
        Entity::Signal signal;
        signal.signal = 0xEB1B2287;
        player->entity->signal(signal);
    }*/

    if (utility::was_key_down(VK_F3)) {
        player->behavior->setSuspend(!player->behavior->isSuspend());
    }
}

void AutomataMPMod::shared_think() {
    //spdlog::info("Shared think");

    //static uint32_t(*changePlayer)(Entity* player) = (decltype(changePlayer))0x1401ED500;

    auto entityList = sdk::EntityList::get();

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

    if (!controlledEntity || !controlledEntity->behavior) {
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

    if (controlledEntity->behavior->is_pl0000()) {
        m_mid_hooks.add_overriden_entity(controlledEntity->behavior);
        m_player_hook.re_hook(controlledEntity->behavior->as<sdk::Pl0000>());
        controlledEntity->behavior->obj_flags() = 0;

        auto realBuddy = entityList->getByHandle(controlledEntity->behavior->as<sdk::Pl0000>()->buddy_handle());
    }

    if (m_client) {
        m_client->think();
    }
}
