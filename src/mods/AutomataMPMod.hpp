#pragma once

#include <chrono>
#include <array>

#include "../Mod.hpp"

#include "multiplayer/VehHooks.hpp"
#include "multiplayer/PlayerHook.hpp"

#include "multiplayer/NierServer.hpp"
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

    void on_draw_ui() override;
    void on_frame() override;
    void on_think() override;
    void sharedThink();
    void signalDestroyClient() {
        m_wantsDestroyClient = true;
    }

    auto& getNetworkEntities() {
        return m_networkEntities;
    }

    auto& getClient() const {
        return m_client;
    }

public:
    void synchronize();
    void serverPacketProcess(const Packet* data, size_t size);
    void sharedPacketProcess(const Packet* data, size_t size);

private:
    void processPlayerData(const nier_client_and_server::PlayerData* movement);
    void processAnimationStart(const nier_client_and_server::AnimationStart* animation);
    void processButtons(const nier_client_and_server::Buttons* buttons);
    void processEntitySpawn(nier_server::EntitySpawn* spawn);
    void processEntityData(nier_server::EntityData* data);

private:
    std::chrono::high_resolution_clock::time_point m_nextThink;

    bool m_isServer{ false };
    bool m_wantsDestroyClient{false};
    
    std::mutex m_hookGuard;

    VehHooks m_vehHooks;
    PlayerHook m_playerHook;

    std::unique_ptr<NierClient> m_client;
    std::unique_ptr<NierServer> m_server;

    EntitySync m_networkEntities;

private:
    // imgui stuff
    std::array<char, 256> m_ip_connect_input{};
    std::array<char, 256> m_password_input{};
    std::array<char, 256> m_name_input{};
};
