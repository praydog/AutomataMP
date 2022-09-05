#include <thread>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <sdk/CameraGame.hpp>
#include <sdk/Enums.hpp>

#include "AutomataMP.hpp"

#include "schema/Packets_generated.h"
#include "mods/AutomataMPMod.hpp"
#include "NierClient.hpp"

using namespace std;

NierClient::NierClient(const std::string& host, const std::string& port, const std::string& name, const std::string& password)
    : m_hello_name{ name },
    m_password{ password }
{
    std::scoped_lock _{m_mtx};

    enetpp::global_state::get().deinitialize();
    enetpp::global_state::get().initialize();

    set_trace_handler([](const std::string& s) { spdlog::info("{}", s); });
    
    enet_uint16 port_num = static_cast<enet_uint16>(std::stoi(port));
    connect(enetpp::client_connect_params().set_channel_count(1).set_server_host_name_and_port(host.c_str(), port_num).set_timeout(chrono::seconds(1)));

    while (get_connection_state() == enetpp::CONNECT_CONNECTING) {
        think();
        this_thread::yield();
    }
}

NierClient::~NierClient() {
    std::scoped_lock _{m_mtx};
    disconnect();
}

void NierClient::think() {
    std::scoped_lock _{m_mtx};

    consume_events(
        [this]() { on_connect(); },
        [this]() { on_disconnect(); },
        [this](const enet_uint8* a, size_t b) { 
            on_data_received(a, b); 
        }
    );

    if (m_hello_sent && m_welcome_received && m_players.contains(m_guid)) {
        update_local_player_data();
        send_player_data();

        // Synchronize the players.
        for (auto& it : m_players) {
            const auto& networked_player = it.second;

            // Do not update the local player here.
            if (networked_player == nullptr || networked_player->get_guid() == m_guid) {
                continue;
            }

            auto npc = networked_player->get_entity();

            if (npc == nullptr) {
                spdlog::error("NPC for player {} not found", networked_player->get_guid());
                continue;
            } 

            //spdlog::info("Synchronizing player {}", networkedPlayer->get_guid());

            auto& data = networked_player->get_player_data();
            npc->run_speed_type() = regenny::ERunSpeedType::SPEED_PLAYER;
            npc->flashlight() = data.flashlight();
            npc->speed() = data.speed();
            npc->facing() = data.facing();
            npc->facing2() = data.facing2();
            npc->weapon_index() = data.weapon_index();
            npc->pod_index() = data.pod_index();
            npc->character_controller().held_flags = data.held_button_flags();
            //*npc->getPosition() = *(Vector3f*)&data.position();
        }

        m_network_entities->think();
    }
}

void NierClient::on_draw_ui() {
    std::scoped_lock _{m_players_mutex};

    for (auto& it : m_players) {
        if (it.second->get_guid() == m_guid) {
            continue;
        }

        if (ImGui::TreeNode(it.second->get_name().c_str())) {
            if (ImGui::Button("Teleport To")) {
                auto ents = sdk::EntityList::get();
                auto controlled = ents->get_possessed_entity();

                if (controlled != nullptr && controlled->behavior != nullptr) {
                    if (controlled->behavior->is_pl0000()) {
                        controlled->behavior->as<sdk::Pl0000>()->setPosRotResetHap(Vector4f{*(Vector3f*)&it.second->get_player_data().position(), 1.0f}, glm::identity<glm::quat>());
                    } else {
                        controlled->behavior->position() = *(Vector3f*)&it.second->get_player_data().position();
                    }
                }
            }

            ImGui::TreePop();
        }
    }
}

void NierClient::on_frame() {
    std::scoped_lock _{m_players_mutex};

    const auto size = g_framework->get_d3d11_rt_size();
    const auto camera = sdk::CameraGame::get();

    for (auto& it : m_players) {
        if (it.second->get_guid() == m_guid) {
            continue;
        }

        if (it.second->get_entity() == nullptr) {
            continue;
        }

        const auto s = camera->world_to_screen(size, it.second->get_entity()->position());

        if (s) {
            ImGui::GetBackgroundDrawList()->AddText(
                ImGui::GetFont(),
                ImGui::GetFontSize(),
                ImVec2{s->x, s->y + 1},
                0xFF000000,
                it.second->get_name().c_str());

            ImGui::GetBackgroundDrawList()->AddText(
                ImGui::GetFont(),
                ImGui::GetFontSize(),
                ImVec2{s->x, s->y -1},
                0xFF000000,
                it.second->get_name().c_str());

            ImGui::GetBackgroundDrawList()->AddText(
                ImGui::GetFont(),
                ImGui::GetFontSize(),
                ImVec2{s->x - 1, s->y},
                0xFF000000,
                it.second->get_name().c_str());

            ImGui::GetBackgroundDrawList()->AddText(
                ImGui::GetFont(),
                ImGui::GetFontSize(),
                ImVec2{s->x + 1, s->y},
                0xFF000000,
                it.second->get_name().c_str());

            ImGui::GetBackgroundDrawList()->AddText(
                ImGui::GetFont(),
                ImGui::GetFontSize(),
                *(ImVec2*)&*s,
                ImGui::GetColorU32(ImGuiCol_Text),
                it.second->get_name().c_str());
        }
    }
}

void NierClient::on_connect() {
    if (auto ents = sdk::EntityList::get(); ents == nullptr || ents->get_possessed_entity() == nullptr) {
        AutomataMPMod::get()->signal_destroy_client();
        spdlog::error("Please spawn a player before connecting to the server.");
        return;
    }

    //spdlog::set_default_logger(spdlog::basic_logger_mt("AutomataMPClient", "automatamp_clientlog.txt", true));
    spdlog::info("Connected");

    if (!m_welcome_received && !m_hello_sent) {
        send_hello();
    }
}

void NierClient::on_disconnect() {
    spdlog::info("Disconnected");
}

void NierClient::on_data_received(const enet_uint8* data, size_t size) {
    try {
        auto verif = flatbuffers::Verifier(data, size);
        const auto packet = flatbuffers::GetRoot<nier::Packet>(data);

        if (!packet->Verify(verif)) {
            spdlog::error("Invalid packet");
            return;
        }

        on_packet_received(packet);
    } catch(const std::exception& e) {
        spdlog::error("Exception occurred during packet processing: {}", e.what());
    } catch(...) {
        spdlog::error("Unknown exception occurred during packet processing");
    }
}

void NierClient::on_packet_received(const nier::Packet* packet) {
    if (!m_welcome_received && packet->id() != nier::PacketType_ID_WELCOME) {
        spdlog::error("Expected welcome packet, but got {} ({}), ignoring", packet->id(), nier::EnumNamePacketType(packet->id()));
        return;
    }

    const nier::PlayerPacket* player_packet = nullptr;

    // Bounced player packets.
    if (packet->id() > nier::PacketType_ID_CLIENT_START && packet->id() < nier::PacketType_ID_CLIENT_END) {
        player_packet = flatbuffers::GetRoot<nier::PlayerPacket>(packet->data()->data());
        flatbuffers::Verifier player_verif(packet->data()->data(), packet->data()->size());

        if (!player_packet->Verify(player_verif)) {
            spdlog::error("Invalid player packet {} ({})", packet->id(), nier::EnumNamePacketType(packet->id()));
            return;
        }

        on_player_packet_received(packet->id(), player_packet);
        return;
    }

    // Standard packets.
    switch(packet->id()) {
        case nier::PacketType_ID_WELCOME: {
            if (handle_welcome(packet)) {
                m_welcome_received = true;
            }

            break;
        }

        case nier::PacketType_ID_SET_MASTER_CLIENT: {
            m_is_master_client = true;
            break;
        }

        case nier::PacketType_ID_CREATE_PLAYER: {
            if (!handle_create_player(packet)) {
                spdlog::error("Failed to create player");
            }

            break;
        }

        case nier::PacketType_ID_DESTROY_PLAYER: {
            if (!handle_destroy_player(packet)) {
                spdlog::error("Failed to destroy player");
            }

            break;
        }
        
        case nier::PacketType_ID_SPAWN_ENTITY: [[fallthrough]];
        case nier::PacketType_ID_DESTROY_ENTITY: [[fallthrough]];
        case nier::PacketType_ID_ENTITY_DATA: [[fallthrough]];
        case nier::PacketType_ID_ENTITY_ANIMATION_START: {
            const auto entity_packet = flatbuffers::GetRoot<nier::EntityPacket>(packet->data()->data());
            flatbuffers::Verifier entity_verif(packet->data()->data(), packet->data()->size());

            if (!entity_packet->Verify(entity_verif)) {
                spdlog::error("Invalid entity packet {} ({})", packet->id(), nier::EnumNamePacketType(packet->id()));
                return;
            }

            on_entity_packet_received(packet->id(), entity_packet);
            break;
        }

        default:
            spdlog::error("Unknown packet type {} ({})", packet->id(), nier::EnumNamePacketType(packet->id()));
            break;
    }

    /*if (data->id >= ID_SHARED_START && data->id < ID_SHARED_END) {
        AutomataMPMod::get()->sharedPacketProcess(data, size);
    }
    else if (data->id >= ID_SERVER_START && data->id < ID_SERVER_END) {
        AutomataMPMod::get()->serverPacketProcess(data, size);
    }*/
}

void NierClient::on_player_packet_received(nier::PacketType packet_type, const nier::PlayerPacket* packet) {
    spdlog::info("Player packet {} received from {}", nier::EnumNamePacketType(packet_type), packet->guid());

    switch (packet_type) {
    case nier::PacketType_ID_PLAYER_DATA: {
        if (!handle_player_data(packet)) {
            spdlog::error("Failed to handle player data");
        }

        break;
    }
    case nier::PacketType_ID_ANIMATION_START: {
        if (!handle_animation_start(packet)) {
            spdlog::error("Failed to handle animation start");
        }

        break;
    }
    case nier::PacketType_ID_BUTTONS: {
        if (!handle_buttons(packet)) {
            spdlog::error("Failed to handle buttons");
        }

        break;
    }
    default:
        spdlog::error("Unknown player packet type {} ({})", packet_type, nier::EnumNamePacketType(packet_type));
        break;
    }
}

void NierClient::on_entity_packet_received(nier::PacketType packet_type, const nier::EntityPacket* packet) {
    spdlog::info("Entity packet {} received from {}", nier::EnumNamePacketType(packet_type), packet->guid());

    switch (packet_type) {
    case nier::PacketType_ID_SPAWN_ENTITY: {
        if (!handle_create_entity(packet)) {
            spdlog::error("Failed to handle spawn entity");
        }

        break;
    }
    case nier::PacketType_ID_DESTROY_ENTITY: {
        if (!handle_destroy_entity(packet)) {
            spdlog::error("Failed to handle destroy entity");
        }

        break;
    }
    case nier::PacketType_ID_ENTITY_DATA: {
        if (!handle_entity_data(packet)) {
            spdlog::error("Failed to handle entity data");
        }

        break;
    }
    case nier::PacketType_ID_ENTITY_ANIMATION_START: {
        if (!handle_entity_animation_start(packet)) {
            spdlog::error("Failed to handle entity animation start");
        }

        break;
    }
    default:
        spdlog::error("Unknown entity packet type {} ({})", packet_type, nier::EnumNamePacketType(packet_type));
        break;
    }
}

void NierClient::send_packet(nier::PacketType id, const uint8_t* data, size_t size) {
    auto builder = flatbuffers::FlatBufferBuilder{};

    uint32_t dataoffs = 0;

    if (data != nullptr && size > 0) {
        builder.StartVector(size, 1); // byte vector
        for (int64_t i = (int64_t)size - 1; i >= 0; i--) {
            builder.PushElement(data[i]);
        }
        dataoffs = builder.EndVector(size);
    }

    auto packet_builder = nier::PacketBuilder(builder);

    packet_builder.add_magic(1347240270);
    packet_builder.add_id(id);

    if (data != nullptr && size > 0) {
        packet_builder.add_data(dataoffs);
    }

    builder.Finish(packet_builder.Finish());

    this->enetpp::client::send_packet(0, builder.GetBufferPointer(), builder.GetSize(), ENET_PACKET_FLAG_RELIABLE);
}

void NierClient::send_animation_start(uint32_t anim, uint32_t variant, uint32_t a3, uint32_t a4) {
    nier::AnimationStart data{anim, variant, a3, a4};

    flatbuffers::FlatBufferBuilder builder(0);
    auto dataoffs = builder.CreateStruct(data);
    builder.Finish(dataoffs);

    send_packet(nier::PacketType_ID_ANIMATION_START, builder.GetBufferPointer(), builder.GetSize());
}

void NierClient::send_buttons(const uint32_t* buttons) {
    flatbuffers::FlatBufferBuilder builder(0);
    const auto dataoffs = builder.CreateVector(buttons, sdk::Pl0000::EButtonIndex::INDEX_MAX);

    nier::Buttons::Builder data_builder(builder);
    data_builder.add_buttons(dataoffs);
    builder.Finish(data_builder.Finish());

    send_packet(nier::PacketType_ID_BUTTONS, builder.GetBufferPointer(), builder.GetSize());
}

void NierClient::send_entity_packet(nier::PacketType id, uint32_t guid, const uint8_t* data, size_t size) {
    flatbuffers::FlatBufferBuilder builder(0);
    const auto dataoffs = builder.CreateVector(data, size);

    nier::EntityPacket::Builder data_builder(builder);
    data_builder.add_guid(guid);
    data_builder.add_data(dataoffs);
    builder.Finish(data_builder.Finish());

    send_packet(id, builder.GetBufferPointer(), builder.GetSize());
}

void NierClient::send_entity_create(uint32_t guid, sdk::EntitySpawnParams* data) {
    if (!m_is_master_client) {
        spdlog::info("Not master client, not sending entity create");
        return;
    }

    // entity packet.
    flatbuffers::FlatBufferBuilder builder(0);
    const auto name = builder.CreateString(data->name);

    nier::EntitySpawnParams::Builder data_builder(builder);
    data_builder.add_name(name);
    data_builder.add_model(data->model);
    data_builder.add_model2(data->model2);

    if (data->matrix != nullptr) {
        data_builder.add_positional((nier::EntitySpawnPositionalData*)data->matrix);
    }

    builder.Finish(data_builder.Finish());

    send_entity_packet(nier::PacketType_ID_SPAWN_ENTITY, guid, builder.GetBufferPointer(), builder.GetSize());
}

void NierClient::send_entity_destroy(uint32_t guid) {
    if (!m_is_master_client) {
        spdlog::info("Not master client, not sending entity destroy");
        return;
    }

    send_entity_packet(nier::PacketType_ID_DESTROY_ENTITY, guid);
}

void NierClient::send_entity_data(uint32_t guid, sdk::BehaviorAppBase* entity) {
    if (!m_is_master_client) {
        spdlog::info("Not master client, not sending entity data");
        return;
    }

    flatbuffers::FlatBufferBuilder builder(0);
    nier::EntityData new_data(entity->facing(),
        0.0f, // entity is not a player.
        entity->health(), *(nier::Vector3f*)&entity->position());

    builder.Finish(builder.CreateStruct(new_data));

    m_network_entities->process_entity_data(guid, &new_data);
    send_entity_packet(nier::PacketType_ID_ENTITY_DATA, guid, builder.GetBufferPointer(), builder.GetSize());
}

void NierClient::send_entity_animation_start(uint32_t guid, uint32_t anim, uint32_t variant, uint32_t a3, uint32_t a4) {
    if (!m_is_master_client) {
        spdlog::info("Not master client, not sending entity animation start");
        return;
    }
    
    flatbuffers::FlatBufferBuilder builder(0);
    nier::AnimationStart data{anim, variant, a3, a4};
    builder.Finish(builder.CreateStruct(data));

    send_entity_packet(nier::PacketType_ID_ENTITY_ANIMATION_START, guid, builder.GetBufferPointer(), builder.GetSize());
}

void NierClient::on_entity_created(sdk::Entity* entity, sdk::EntitySpawnParams* data) {
    if (!m_is_master_client) {
        entity->behavior->terminate(); // destroy the entity. only the server or the master client should create entities.
        return;
    }

    m_network_entities->on_entity_created(entity, data);
}

void NierClient::on_entity_deleted(sdk::Entity* entity) {
    m_network_entities->on_entity_deleted(entity);
}

void NierClient::send_hello() {
    auto ents = sdk::EntityList::get();
    auto possessed = ents->get_possessed_entity();

    if (possessed == nullptr || possessed->behavior == nullptr) {
        spdlog::error("No possessed entity");
        return;
    }
    

    flatbuffers::FlatBufferBuilder builder{};
    const auto name_pkt = builder.CreateString(m_hello_name);
    const auto pwd_pkt = builder.CreateString(m_password);

    nier::HelloBuilder hello_builder(builder);
    hello_builder.add_major(nier::VersionMajor_Value);
    hello_builder.add_minor(nier::VersionMinor_Value);
    hello_builder.add_patch(nier::VersionPatch_Value);
    hello_builder.add_name(name_pkt);
    hello_builder.add_password(pwd_pkt);
    hello_builder.add_model(possessed->behavior->model_index());

    builder.Finish(hello_builder.Finish());

    send_packet(nier::PacketType_ID_HELLO, builder.GetBufferPointer(), builder.GetSize());
    m_hello_sent = true;
}

void NierClient::update_local_player_data() {
    if (!m_hello_sent || !m_welcome_received || m_guid == 0) {
        return;
    }

    auto it = m_players.find(m_guid);

    if (it == m_players.end() || it->second == nullptr) {
        spdlog::error("Local player not set up");
        return;
    }

    auto entity_list = sdk::EntityList::get();

    if (entity_list == nullptr) {
        return;
    }

    auto player = entity_list->get_possessed_entity();

    if (player == nullptr) {
        return;
    }

    it->second->set_handle(player->handle);
}

void NierClient::send_player_data() {
    if (m_guid == 0) {
        spdlog::error("Cannot send player data without GUID");
        return;
    }

    auto it = m_players.find(m_guid);

    if (it == m_players.end() || it->second == nullptr) {
        spdlog::error("Cannot send player data without player");
        return;
    }

    auto& player = it->second;
    
    auto entity = player->get_entity();

    if (entity == nullptr) {
        spdlog::error("Cannot send player data without entity");
        return;
    }

    nier::PlayerData player_data(entity->flashlight(), entity->speed(), entity->facing(), entity->facing2(), entity->weapon_index(),
        entity->pod_index(), entity->character_controller().held_flags, *(nier::Vector3f*)&entity->position());

    flatbuffers::FlatBufferBuilder builder{};
    const auto offs = builder.CreateStruct(player_data);
    builder.Finish(offs);

    send_packet(nier::PacketType_ID_PLAYER_DATA, builder.GetBufferPointer(), builder.GetSize());
}

bool NierClient::handle_welcome(const nier::Packet* packet) {
    spdlog::info("Welcome packet received");

    const auto welcome = flatbuffers::GetRoot<nier::Welcome>(packet->data()->data());
    auto verif = flatbuffers::Verifier(packet->data()->data(), packet->data()->size());

    if (!welcome->Verify(verif)) {
        spdlog::error("Invalid welcome packet");
        return false;
    }

    m_is_master_client = welcome->isMasterClient();
    m_guid = welcome->guid();
    const auto highest_guid = welcome->highestEntityGuid();

    spdlog::info("Welcome packet received, isMasterClient: {}, guid: {}", m_is_master_client, m_guid);

    m_network_entities = std::make_unique<EntitySync>(highest_guid);
    m_network_entities->on_enter_server(m_is_master_client);

    return true;
}

bool NierClient::handle_create_player(const nier::Packet* packet) {
    spdlog::info("Create player packet received");

    auto entity_list = sdk::EntityList::get();

    if (entity_list == nullptr) {
        spdlog::error("Entity list not found while handling create player packet");
        return false;
    }

    auto possessed = entity_list->get_possessed_entity();

    if (possessed == nullptr) {
        spdlog::error("Possessed entity not found while handling create player packet");
        return false;
    }

    auto localplayer = entity_list->get_by_name("Player");

    if (localplayer == nullptr || localplayer->behavior == nullptr) {
        spdlog::info("Player not found while handling create player packet");
        return false;
    }

    const auto create_player = flatbuffers::GetRoot<nier::CreatePlayer>(packet->data()->data());
    auto verif = flatbuffers::Verifier(packet->data()->data(), packet->data()->size());

    if (!create_player->Verify(verif)) {
        spdlog::error("Invalid create player packet");
        return false;
    }

    {
        std::scoped_lock _{m_players_mutex};

        auto new_player = std::make_unique<Player>();
        new_player->set_guid(create_player->guid());
        new_player->set_name(create_player->name()->c_str());

        m_players[create_player->guid()] = std::move(new_player);
    }

    // we don't want to spawn ourselves
    if (create_player->guid() != m_guid) {
        spdlog::info("Spawning player {}, {}", create_player->guid(), create_player->name()->c_str());

        MidHooks::s_ignore_spawn = true;
        auto ent = entity_list->spawn_entity("partner", create_player->model(), possessed->behavior->position());
        MidHooks::s_ignore_spawn = false;

        if (ent != nullptr) {
            std::scoped_lock _{m_players_mutex};

            spdlog::info(" Player spawned");

            ent->behavior->as<sdk::Pl0000>()->buddy_handle() = localplayer->handle;
            localplayer->behavior->as<sdk::Pl0000>()->buddy_handle() = ent->handle;

            ent->behavior->setSuspend(false);

            ent->assign_ai_routine("PLAYER");
            ent->assign_ai_routine("player");

            // alternate way of assigning AI/control to the entity easily.
            localplayer->behavior->as<sdk::Pl0000>()->changePlayer();
            localplayer->behavior->as<sdk::Pl0000>()->changePlayer();

            ent->behavior->obj_flags() = -1;
            ent->behavior->as<sdk::Pl0000>()->setBuddyFromNpc();
            ent->behavior->obj_flags() = 0;

            m_players[create_player->guid()]->set_start_tick(ent->behavior->tick_count());
            m_players[create_player->guid()]->set_handle(ent->handle);

            spdlog::info(" player assigned handle {:x}", ent->handle);
        } else {
            spdlog::error("Failed to spawn partner");
        }
    } else {
        spdlog::info("not spawning self");
    }

    return true;
}

bool NierClient::handle_destroy_player(const nier::Packet* packet) {
    spdlog::info("Destroy player packet received");

    const auto destroy_player = flatbuffers::GetRoot<nier::DestroyPlayer>(packet->data()->data());

    std::scoped_lock _{m_players_mutex};

    if (m_players.contains(destroy_player->guid()) && m_players[destroy_player->guid()] != nullptr) {
        auto entity_list = sdk::EntityList::get();

        if (entity_list == nullptr) {
            // not an error, we just won't actually delete any entity from the entity list
            spdlog::info("Entity list not found while handling destroy player packet");
        } else {
            auto localplayer = entity_list->get_by_name("Player");
            auto ent = entity_list->get_by_handle(m_players[destroy_player->guid()]->get_handle());
            if (ent != nullptr && ent != localplayer) {
                ent->behavior->terminate();
            }
        }
    }

    m_players[destroy_player->guid()].reset();
    m_players.erase(destroy_player->guid());

    return true;
}

bool NierClient::handle_create_entity(const nier::EntityPacket* packet) {
    spdlog::info("Create entity packet received");

    const auto spawn = flatbuffers::GetRoot<nier::EntitySpawnParams>(packet->data()->data());
    auto verif = flatbuffers::Verifier(packet->data()->data(), packet->data()->size());

    if (!spawn->Verify(verif)) {
        spdlog::error("Invalid create entity packet");
        return false;
    }

    auto entity_list = sdk::EntityList::get();

    if (entity_list != nullptr) {
        sdk::EntitySpawnParams params{};
        auto matrix = spawn->positional() != nullptr ? *(sdk::EntitySpawnParams::PositionalData*)spawn->positional() : sdk::EntitySpawnParams::PositionalData{};
        params.matrix = &matrix;
        params.model = spawn->model();
        params.model2 = spawn->model2();
        params.name = spawn->name()->c_str();

        spdlog::info(" Spawning {}", spawn->name()->c_str());

        //const auto pos = spawn->positional() != nullptr ? *(Vector3f*)&spawn->positional()->position() : Vector3f{};
        //auto ent = entityList->spawnEntity(spawn->name()->c_str(), spawn->model(), pos);

        // Allows the client to spawn an entity.
        MidHooks::s_ignore_spawn = true;
        auto ent = entity_list->spawn_entity(params);
        MidHooks::s_ignore_spawn = false;

        if (ent != nullptr) {
            //ent->entity->setSuspend(false);

            spdlog::info(" Entity spawned @ {:x}", (uintptr_t)ent);
            auto new_network_ent = m_network_entities->add_entity(ent, packet->guid());

            if (new_network_ent != nullptr) {
                spdlog::info(" Network entity created");
            }
        } else {
            spdlog::error(" Failed to spawn entity");
        }
    }

    return true;
}

bool NierClient::handle_destroy_entity(const nier::EntityPacket* packet) {
    spdlog::info("Destroy entity packet received");

    m_network_entities->remove_entity(packet->guid());

    return true;
}

bool NierClient::handle_entity_data(const nier::EntityPacket* packet) {
    spdlog::info("Entity data packet received");

    const auto entity_data = flatbuffers::GetRoot<nier::EntityData>(packet->data()->data());
    m_network_entities->process_entity_data(packet->guid(), entity_data);

    return true;
}

bool NierClient::handle_entity_animation_start(const nier::EntityPacket* packet) {
    spdlog::info("Entity animation start packet received");

    const auto guid = packet->guid();
    auto entity_networked = m_network_entities->get_network_entity_from_guid(guid);

    if (entity_networked == nullptr) {
        spdlog::error(" (nullptr) Entity data packet received for unknown entity {}", guid);
        return false;
    }

    auto animation_data = flatbuffers::GetRoot<nier::AnimationStart>(packet->data()->data());
    auto npc = entity_networked->get_entity() != nullptr ? entity_networked->get_entity()->behavior : nullptr;

    if (npc != nullptr) {
        switch (animation_data->anim()) {
        case sdk::EAnimation::INVALID_CRASHES_GAME:
        case sdk::EAnimation::INVALID_CRASHES_GAME2:
        case sdk::EAnimation::INVALID_CRASHES_GAME3:
        case sdk::EAnimation::INVALID_CRASHES_GAME4:
            return true;
        default:
            if (npc) {
                npc->start_animation(animation_data->anim(), animation_data->variant(), animation_data->a3(), animation_data->a4());
            } else {
                spdlog::error(" Cannot start animation, npc is null");
            }
        }
    }

    return true;
}

bool NierClient::handle_player_data(const nier::PlayerPacket* packet) {
    const auto guid = packet->guid();

    // do not update the local player. maybe change this later for forced updates/teleportation commands?
    if (guid == m_guid) {
        return true;
    }

    if (!m_players.contains(guid)) {
        spdlog::error("Player data packet received for unknown player {}", guid);
        return false;
    }

    const auto& player_networked = m_players[guid];

    if (player_networked == nullptr) {
        spdlog::error("(nullptr) Player data packet received for unknown player {}", guid);
        return false;
    }

    auto player_data = flatbuffers::GetRoot<nier::PlayerData>(packet->data()->data());
    auto npc = player_networked->get_entity();

    if (npc != nullptr) {
        npc->position() = *(Vector3f*)&player_data->position();
    }

    player_networked->set_player_data(*player_data);

    return true;
}

bool NierClient::handle_animation_start(const nier::PlayerPacket* packet) {
    const auto guid = packet->guid();

    // do not update the local player. maybe change this later for forced updates/teleportation commands?
    if (guid == m_guid) {
        return true;
    }

    if (!m_players.contains(guid)) {
        spdlog::error("Player data packet received for unknown player {}", guid);
        return false;
    }

    const auto& player_networked = m_players[guid];

    if (player_networked == nullptr) {
        spdlog::error("(nullptr) Player data packet received for unknown player {}", guid);
        return false;
    }

    auto animation_data = flatbuffers::GetRoot<nier::AnimationStart>(packet->data()->data());
    auto npc = player_networked->get_entity();

    if (npc != nullptr) {
        switch (animation_data->anim()) {
        case sdk::EAnimation::INVALID_CRASHES_GAME:
        case sdk::EAnimation::INVALID_CRASHES_GAME2:
        case sdk::EAnimation::INVALID_CRASHES_GAME3:
        case sdk::EAnimation::INVALID_CRASHES_GAME4:
        case sdk::EAnimation::Light_Attack:
            return true;
        default:
            if (npc) {
                npc->start_animation(animation_data->anim(), animation_data->variant(), animation_data->a3(), animation_data->a4());
            } else {
                spdlog::error("Cannot start animation, npc is null");
            }
        }
    }
    
    return true;
}

bool NierClient::handle_buttons(const nier::PlayerPacket* packet) {
    const auto guid = packet->guid();
    
    // do not update the local player. maybe change this later for forced updates/teleportation commands?
    if (guid == m_guid) {
        return true;
    }

    if (!m_players.contains(guid)) {
        spdlog::error("Player data packet received for unknown player {}", guid);
        return false;
    }

    const auto& player_networked = m_players[guid];

    if (player_networked == nullptr) {
        spdlog::error("(nullptr) Player data packet received for unknown player {}", guid);
        return false;
    }

    auto buttons = flatbuffers::GetRoot<nier::Buttons>(packet->data()->data());
    auto npc = player_networked->get_entity();

    if (npc != nullptr) {
        const auto buttons_data = buttons->buttons()->data();
        const auto size_buttons = sizeof(regenny::CharacterController::buttons);
        memcpy(&npc->character_controller().buttons, buttons_data, size_buttons);

        for (uint32_t i = 0; i < sdk::Pl0000::EButtonIndex::INDEX_MAX; ++i) {
            auto& controller = npc->character_controller();

            if (buttons_data[i] > 0) {
                controller.held_flags |= (1 << i);
            }
        }
    }

    return true;
}
