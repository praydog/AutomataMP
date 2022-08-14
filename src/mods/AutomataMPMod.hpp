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
    void sendPacket(const enet_uint8* data, size_t size);

    bool isServer() {
        return m_client != nullptr && m_client->isMasterClient();
    }

    void onEntityCreated(sdk::Entity* entity, sdk::EntitySpawnParams* data) {
        if (m_client != nullptr) {
            m_client->onEntityCreated(entity, data);
        }
    }
    
    void onEntityDeleted(sdk::Entity* entity) {
        if (m_client != nullptr) {
            m_client->onEntityDeleted(entity);
        }
    }

    void on_draw_ui() override;
    void on_frame() override;
    void on_think() override;
    void sharedThink();
    void signalDestroyClient() {
        m_wantsDestroyClient = true;
    }

    auto& getClient() const {
        return m_client;
    }

private:
    std::chrono::high_resolution_clock::time_point m_nextThink;

    bool m_isServer{ false };
    bool m_wantsDestroyClient{false};
    
    std::mutex m_hookGuard;

    MidHooks m_midHooks;
    PlayerHook m_playerHook;

    std::unique_ptr<NierClient> m_client;

private:
    void display_servers();
    struct ServerData {
        std::string ip;
        std::string name;
        uint32_t numPlayers;
    };

    std::vector<std::unique_ptr<ServerData>> m_servers;
    std::chrono::steady_clock::time_point m_lastServerUpdate{};
    std::future<std::string> m_serverFuture;

    // imgui stuff
    std::array<char, 256> m_ip_connect_input{};
    std::array<char, 256> m_password_input{};
    std::array<char, 256> m_name_input{};
    std::array<char, 256> m_master_server_input{};
};
