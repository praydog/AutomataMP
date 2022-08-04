#pragma once

#include <unordered_map>

#include <enetpp/client.h>

#include "Player.hpp"
#include "schema/Packets_generated.h"

struct Packet;

class NierClient : public enetpp::client {
public:
    NierClient(
        const std::string& host, 
        const std::string& name = "Client",
        const std::string& password = "");
    ~NierClient();

    void think();
    void on_draw_ui();
    bool isConnected() { return get_connection_state() == enetpp::CONNECT_CONNECTED; }
    
    const auto getGuid() const {
        return m_guid;
    }

    const auto isMasterClient() const {
        return m_isMasterClient;
    }

    const auto& getPlayers() const {
        return m_players;
    }

private:
    void onConnect();
    void onDisconnect();
    void onDataReceived(const enet_uint8* data, size_t size);
    void onPacketReceived(const nier::Packet* packet);
    void onPlayerPacketReceived(nier::PacketType packetType, const nier::PlayerPacket* packet);

    void sendPacket(nier::PacketType id, const uint8_t* data = nullptr, size_t size = 0);
    void sendHello();
    bool handleWelcome(const nier::Packet* packet);
    bool handleCreatePlayer(const nier::Packet* packet);
    bool handleDestroyPlayer(const nier::Packet* packet);
    bool handlePlayerData(const nier::PlayerPacket* packet);


    std::string m_helloName{};
    std::string m_password{};

    bool m_welcomeReceived{ false };
    bool m_helloSent{ false };

    bool m_isMasterClient{false};
    uint64_t m_guid{};

    std::unordered_map<uint64_t, std::unique_ptr<Player>> m_players{};
};