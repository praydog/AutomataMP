#pragma once

#include <chrono>
#include <array>

#include "../Mod.hpp"

#include "multiplayer/MidHooks.hpp"
#include "multiplayer/PlayerHook.hpp"

#include "multiplayer/NierClient.hpp"
#include "multiplayer/Packets.hpp"
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
    bool clientConnect();
    void serverStart();
    void sendPacket(const enet_uint8* data, size_t size);

    bool isServer() {
        return m_client != nullptr && m_client->isMasterClient();
    }

    void onEntityCreated(EntityContainer* entity, EntitySpawnParams* data) {
        if (m_client != nullptr) {
            m_client->onEntityCreated(entity, data);
        }
    }
    
    void onEntityDeleted(EntityContainer* entity) {
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
    // imgui stuff
    std::array<char, 256> m_ip_connect_input{};
    std::array<char, 256> m_password_input{};
    std::array<char, 256> m_name_input{};
};
