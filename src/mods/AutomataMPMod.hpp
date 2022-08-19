#pragma once

#include <chrono>
#include <array>
#include <future>

#include "../Mod.hpp"

#include "multiplayer/MidHooks.hpp"
#include "multiplayer/PlayerHook.hpp"

#include "multiplayer/NierClient.hpp"
#include "multiplayer/Player.hpp"
#include "multiplayer/EntitySync.hpp"

class AutomataMPMod : public Mod {
public:
    static std::shared_ptr<AutomataMPMod> get();
public:

    ~AutomataMPMod();

    std::string_view get_name() const override { return "AutomataMPMod"; }
    std::optional<std::string> on_initialize() override;

public:
    bool is_server() {
        return m_client != nullptr && m_client->is_master_client();
    }

    void on_entity_created(sdk::Entity* entity, sdk::EntitySpawnParams* data) {
        if (m_client != nullptr) {
            m_client->on_entity_created(entity, data);
        }
    }
    
    void on_entity_deleted(sdk::Entity* entity) {
        if (m_client != nullptr) {
            m_client->on_entity_deleted(entity);
        }
    }

    void on_draw_ui() override;
    void on_frame() override;
    void on_think() override;
    void shared_think();
    void signal_destroy_client() {
        m_wants_destroy_client = true;
    }

    auto& get_client() const {
        return m_client;
    }

private:
    std::chrono::high_resolution_clock::time_point m_next_think;

    bool m_is_server{ false };
    bool m_wants_destroy_client{false};
    
    std::mutex m_hook_guard;

    MidHooks m_mid_hooks;
    PlayerHook m_player_hook;

    std::unique_ptr<NierClient> m_client;

private:
    void display_servers();
    struct ServerData {
        std::string ip;
        std::string name;
        uint32_t num_players;
    };

    std::vector<std::unique_ptr<ServerData>> m_servers;
    std::chrono::steady_clock::time_point m_last_server_update{};
    std::future<std::string> m_server_future;

    // imgui stuff
    std::array<char, 256> m_ip_connect_input{};
    std::array<char, 256> m_password_input{};
    std::array<char, 256> m_name_input{};
    std::array<char, 256> m_master_server_input{};
};
