#pragma once

#include <unordered_map>

#include <enetpp/client.h>

#include "Player.hpp"
#include "EntitySync.hpp"
#include "schema/Packets_generated.h"

struct Packet;

class NierClient : public enetpp::client {
public:
    NierClient(
        const std::string& host,
        const std::string& port = "6969",
        const std::string& name = "Client",
        const std::string& password = "");
    virtual ~NierClient();

    void think();
    void on_draw_ui();
    void on_frame();
    bool is_connected() { return get_connection_state() == enetpp::CONNECT_CONNECTED; }

    void send_packet(nier::PacketType id, const uint8_t* data = nullptr, size_t size = 0);
    void send_animation_start(uint32_t anim, uint32_t variant, uint32_t a3, uint32_t a4);
    void send_buttons(const uint32_t* buttons);

    void send_entity_packet(nier::PacketType id, uint32_t guid, const uint8_t* data = nullptr, size_t size = 0);
    void send_entity_create(uint32_t guid, sdk::EntitySpawnParams* data);
    void send_entity_destroy(uint32_t guid);
    void send_entity_data(uint32_t guid, sdk::BehaviorAppBase* entity);
    void send_entity_animation_start(uint32_t guid, uint32_t anim, uint32_t variant, uint32_t a3, uint32_t a4);

    void on_entity_created(sdk::Entity* entity, sdk::EntitySpawnParams* data);
    void on_entity_deleted(sdk::Entity* entity);
    
    const auto get_guid() const {
        return m_guid;
    }

    const auto is_master_client() const {
        return m_is_master_client;
    }

    const auto& get_players() const {
        return m_players;
    }

private:
    void on_connect();
    void on_disconnect();
    void on_data_received(const enet_uint8* data, size_t size);
    void on_packet_received(const nier::Packet* packet);
    void on_player_packet_received(nier::PacketType packet_type, const nier::PlayerPacket* packet);
    void on_entity_packet_received(nier::PacketType packet_type, const nier::EntityPacket* packet);

    void send_hello();

    void update_local_player_data();
    void send_player_data();

    bool handle_welcome(const nier::Packet* packet);
    bool handle_create_player(const nier::Packet* packet);
    bool handle_destroy_player(const nier::Packet* packet);

    bool handle_create_entity(const nier::EntityPacket* packet);
    bool handle_destroy_entity(const nier::EntityPacket* packet);
    bool handle_entity_data(const nier::EntityPacket* packet);
    bool handle_entity_animation_start(const nier::EntityPacket* packet);

    bool handle_player_data(const nier::PlayerPacket* packet);
    bool handle_animation_start(const nier::PlayerPacket* packet);
    bool handle_buttons(const nier::PlayerPacket* packet);

    std::unique_ptr<EntitySync> m_network_entities{};

    std::recursive_mutex m_mtx{};
    std::recursive_mutex m_players_mutex{};
    std::string m_hello_name{};
    std::string m_password{};

    bool m_welcome_received{ false };
    bool m_hello_sent{ false };

    bool m_is_master_client{false};
    uint64_t m_guid{};

    std::unordered_map<uint64_t, std::unique_ptr<Player>> m_players{};
};