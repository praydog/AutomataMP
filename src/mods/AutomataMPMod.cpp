#include <mutex>
#include <regex>

#include <windows.h>

#include <spdlog/spdlog.h>

#include <utility/Input.hpp>
#include <utility/HttpClient.hpp>
#include <json.hpp>

#include <sdk/Entity.hpp>
#include <sdk/EntityList.hpp>
#include <sdk/Enums.hpp>

#include <sdk/Game.hpp>
#include <sdk/ScriptFunctions.hpp>
#include "AutomataMPMod.hpp"

using namespace std;

// define default values
#define DEFAULT_MASTER "https://niermaster.praydog.com"
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT "6969"
#define DEFAULT_NAME "Client"


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

    std::strcpy(m_ip_connect_input.data(), DEFAULT_IP);
    std::strcpy(m_port_connect_input.data(), DEFAULT_PORT);
    std::strcpy(m_name_input.data(), DEFAULT_NAME);
    std::strcpy(m_master_server_input.data(), DEFAULT_MASTER);

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

        if (response.empty()) {
            spdlog::info("Empty master server response.");
            strcpy(m_master_server_input.data(), DEFAULT_MASTER);
            return;
        }

        const auto response_json = nlohmann::json::parse(response);

        try {
            for (const auto& [ip, jsondata] : response_json.items()) {
                auto new_server_data = std::make_unique<AutomataMPMod::ServerData>();
                const auto data = jsondata["Data"];

                new_server_data->ip = ip;
                new_server_data->port = data["Port"];
                new_server_data->name = data["Name"];
                new_server_data->num_players = data["NumPlayers"];

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
            // if any fields are empty, don't connect.
            if (server->ip.empty() || server->port.empty()) {
                spdlog::error("Empty IP or port.");
                return;
            }

            spdlog::info("Connecting to {}:{}", server->ip, server->port);

			strcpy(m_ip_connect_input.data(), server->ip.c_str());
            strcpy(m_port_connect_input.data(), server->port.c_str());

            m_client.reset();
            
            m_client = make_unique<NierClient>(server->ip.data(), server->port.data(), m_name_input.data(), m_password_input.data());

            if (!m_client->is_connected()) {
                m_client.reset();
            }

            return;
        }

        ImGui::SameLine();

        const auto made = ImGui::TreeNode((server->name + " (" + std::to_string(server->num_players) + " players)").c_str());

        if (made) {
            ImGui::Text("IP: %s", server->ip.c_str());
            ImGui::Text("Port: %s", server->port.c_str());
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

void AutomataMPMod::display_manual_connect() {
    if (ImGui::Button("Connect") || ImGui::InputText("Connect IP", m_ip_connect_input.data(), m_ip_connect_input.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
        
        m_client.reset();

        // validate against master server
        const auto connection_info = validate_connection(m_ip_connect_input.data(), m_port_connect_input.data());
        const auto val_ip = std::get<0>(connection_info);
        const auto val_port = std::get<1>(connection_info);

        if (val_ip != m_ip_connect_input.data() || val_port != m_port_connect_input.data()) {
            spdlog::error("Invalid IP or port.");
            strcpy(m_ip_connect_input.data(), val_ip.c_str());
            strcpy(m_port_connect_input.data(), val_port.c_str());
            return;
        }

        m_client = make_unique<NierClient>(val_ip, val_port, m_name_input.data(), m_password_input.data());

        if (!m_client->is_connected()) {
            m_client.reset();
        }
    }

    ImGui::InputText("Connect Port", m_port_connect_input.data(), m_port_connect_input.size());
    ImGui::InputText("Name", m_name_input.data(), m_name_input.size());
    ImGui::InputText("Password", m_password_input.data(), m_password_input.size());
}

void AutomataMPMod::on_draw_ui() {
    if (!ImGui::CollapsingHeader("AutomataMPMod")) {
        return;
    }

    if (m_client) {
        if (ImGui::Button("Disconnect")) {
            m_client.reset();
            return;
        }
        ImGui::Text("State: Client");
    } else {
        ImGui::Text("State: Disconnected");
    }

	ImGui::InputText("Master Server", m_master_server_input.data(), m_master_server_input.size());
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Servers")) {
        display_servers();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Manual Connection")) {
        display_manual_connect();
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

    auto entity_list = sdk::EntityList::get();

    if (!entity_list) {
        return;
    }

    auto player = entity_list->get_by_name("Player");

    if (!player) {
        spdlog::info("Player not found");
        return;
    }

    auto partners = entity_list->get_all_by_name("partner");
    auto partner = entity_list->get_by_name("partner");

    if (partner) {
        if (utility::was_key_down(VK_F4)) {
            Vector3f* my_pos = Address(player->behavior).get(0x50).as<Vector3f*>();

            for (auto i : partners) {
                if (!i->behavior) {
                    continue;
                }

                Vector3f* vec = Address(i->behavior).get(0x50).as<Vector3f*>();
                *vec = *my_pos;
            }
        }
    }

    if (utility::was_key_down(VK_F6)) {
        player->behavior->as<sdk::Pl0000>()->changePlayer();
        //nier_client_and_server::ChangePlayer change;
        //sendPacket(change.data(), sizeof(change));
    }

    if (utility::was_key_down(VK_F7)) {
        for (auto& i : *entity_list) {
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

    shared_think();

    if (utility::was_key_down(VK_F9)) {
        auto ent = entity_list->spawn_entity("partner", sdk::EModel::MODEL_2B, player->behavior->position());

        if (ent) {
            ent->assign_ai_routine("buddy_2B");
            ent->assign_ai_routine("buddy");

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

    auto entity_list = sdk::EntityList::get();

    if (!entity_list) {
        return;
    }

    // main player entity that game is originally controlling
    auto player = entity_list->get_by_name("Player");

    if (!player) {
        spdlog::info("Player not found");
        return;
    }

    auto controlled_entity = entity_list->get_possessed_entity();

    if (!controlled_entity || !controlled_entity->behavior) {
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

    if (controlled_entity->behavior->is_pl0000()) {
        m_mid_hooks.add_overriden_entity(controlled_entity->behavior);
        m_player_hook.re_hook(controlled_entity->behavior->as<sdk::Pl0000>());
        controlled_entity->behavior->obj_flags() = 0;

        auto real_buddy = entity_list->get_by_handle(controlled_entity->behavior->as<sdk::Pl0000>()->buddy_handle());
    }

    if (m_client) {
        m_client->think();
    }
}

std::tuple<std::string, std::string> AutomataMPMod::validate_connection(std::string ip, std::string port) {
    std::string valid_ip = DEFAULT_IP;
    std::string valid_port = DEFAULT_PORT;
    bool is_port_valid = false;

    if (!std::regex_match(ip, std::regex("^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"))) {
        return { valid_ip, valid_port };
    }

    for (auto& server : m_servers) {
        valid_port = server->port;
        if (server->ip == ip && valid_port == port) {
            is_port_valid = true;
        }
    }

    return { ip, is_port_valid ? port : valid_port };
}
