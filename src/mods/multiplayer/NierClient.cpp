#include <thread>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <sdk/CameraGame.hpp>

#include "AutomataMP.hpp"

#include "schema/Packets_generated.h"
#include "mods/AutomataMPMod.hpp"
#include "NierClient.hpp"

using namespace std;

NierClient::NierClient(const std::string& host, const std::string& name, const std::string& password)
    : m_helloName{ name },
    m_password{ password }
{
    std::scoped_lock _{m_mtx};

    enetpp::global_state::get().deinitialize();
    enetpp::global_state::get().initialize();

    set_trace_handler([](const std::string& s) { spdlog::info("{}", s); });
    connect(enetpp::client_connect_params().set_channel_count(1).set_server_host_name_and_port(host.c_str(), 6969).set_timeout(chrono::seconds(1)));

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
        [this]() { onConnect(); },
        [this]() { onDisconnect(); },
        [this](const enet_uint8* a, size_t b) { 
            onDataReceived(a, b); 
        }
    );

    if (m_helloSent && m_welcomeReceived && m_players.contains(m_guid)) {
        updateLocalPlayerData();
        sendPlayerData();

        // Synchronize the players.
        for (auto& it : m_players) {
            const auto& networkedPlayer = it.second;

            // Do not update the local player here.
            if (networkedPlayer == nullptr || networkedPlayer->getGuid() == m_guid) {
                continue;
            }

            auto npc = networkedPlayer->getEntity();

            if (npc == nullptr) {
                spdlog::error("NPC for player {} not found", networkedPlayer->getGuid());
                continue;
            } 

            //spdlog::info("Synchronizing player {}", networkedPlayer->getGuid());

            auto& data = networkedPlayer->getPlayerData();
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

        m_networkEntities->think();
    }
}

void NierClient::on_draw_ui() {
    std::scoped_lock _{m_playersMtx};

    for (auto& it : m_players) {
        if (it.second->getGuid() == m_guid) {
            continue;
        }

        if (ImGui::TreeNode(it.second->getName().c_str())) {
            if (ImGui::Button("Teleport To")) {
                auto ents = sdk::EntityList::get();
                auto controlled = ents->getPossessedEntity();

                if (controlled != nullptr && controlled->behavior != nullptr) {
                    if (controlled->behavior->is_pl0000()) {
                        controlled->behavior->as<sdk::Pl0000>()->setPosRotResetHap(Vector4f{*(Vector3f*)&it.second->getPlayerData().position(), 1.0f}, glm::identity<glm::quat>());
                    } else {
                        controlled->behavior->position() = *(Vector3f*)&it.second->getPlayerData().position();
                    }
                }
            }

            ImGui::TreePop();
        }
    }
}

void NierClient::on_frame() {
    std::scoped_lock _{m_playersMtx};

    const auto size = g_framework->get_d3d11_rt_size();
    const auto camera = sdk::CameraGame::get();

    for (auto& it : m_players) {
        if (it.second->getGuid() == m_guid) {
            continue;
        }

        if (it.second->getEntity() == nullptr) {
            continue;
        }

        const auto s = camera->worldToScreen(size, it.second->getEntity()->position());

        if (s) {
            ImGui::GetBackgroundDrawList()->AddText(
                ImGui::GetFont(),
                ImGui::GetFontSize(),
                ImVec2{s->x, s->y + 1},
                0xFF000000,
                it.second->getName().c_str());

            ImGui::GetBackgroundDrawList()->AddText(
                ImGui::GetFont(),
                ImGui::GetFontSize(),
                ImVec2{s->x, s->y -1},
                0xFF000000,
                it.second->getName().c_str());

            ImGui::GetBackgroundDrawList()->AddText(
                ImGui::GetFont(),
                ImGui::GetFontSize(),
                ImVec2{s->x - 1, s->y},
                0xFF000000,
                it.second->getName().c_str());

            ImGui::GetBackgroundDrawList()->AddText(
                ImGui::GetFont(),
                ImGui::GetFontSize(),
                ImVec2{s->x + 1, s->y},
                0xFF000000,
                it.second->getName().c_str());

            ImGui::GetBackgroundDrawList()->AddText(
                ImGui::GetFont(),
                ImGui::GetFontSize(),
                *(ImVec2*)&*s,
                ImGui::GetColorU32(ImGuiCol_Text),
                it.second->getName().c_str());
        }
    }
}

void NierClient::onConnect() {
    if (auto ents = sdk::EntityList::get(); ents == nullptr || ents->getPossessedEntity() == nullptr) {
        AutomataMPMod::get()->signalDestroyClient();
        spdlog::error("Please spawn a player before connecting to the server.");
        return;
    }

    spdlog::set_default_logger(spdlog::basic_logger_mt("AutomataMPClient", "automatamp_clientlog.txt", true));
    spdlog::info("Connected");

    if (!m_welcomeReceived && !m_helloSent) {
        sendHello();
    }
}

void NierClient::onDisconnect() {
    spdlog::info("Disconnected");
}

void NierClient::onDataReceived(const enet_uint8* data, size_t size) {
    try {
        auto verif = flatbuffers::Verifier(data, size);
        const auto packet = flatbuffers::GetRoot<nier::Packet>(data);

        if (!packet->Verify(verif)) {
            spdlog::error("Invalid packet");
            return;
        }

        onPacketReceived(packet);
    } catch(const std::exception& e) {
        spdlog::error("Exception occurred during packet processing: {}", e.what());
    } catch(...) {
        spdlog::error("Unknown exception occurred during packet processing");
    }
}

void NierClient::onPacketReceived(const nier::Packet* packet) {
    if (!m_welcomeReceived && packet->id() != nier::PacketType_ID_WELCOME) {
        spdlog::error("Expected welcome packet, but got {} ({}), ignoring", packet->id(), nier::EnumNamePacketType(packet->id()));
        return;
    }

    const nier::PlayerPacket* playerPacket = nullptr;

    // Bounced player packets.
    if (packet->id() > nier::PacketType_ID_CLIENT_START && packet->id() < nier::PacketType_ID_CLIENT_END) {
        playerPacket = flatbuffers::GetRoot<nier::PlayerPacket>(packet->data()->data());
        flatbuffers::Verifier playerVerif(packet->data()->data(), packet->data()->size());

        if (!playerPacket->Verify(playerVerif)) {
            spdlog::error("Invalid player packet {} ({})", packet->id(), nier::EnumNamePacketType(packet->id()));
            return;
        }

        onPlayerPacketReceived(packet->id(), playerPacket);
        return;
    }

    // Standard packets.
    switch(packet->id()) {
        case nier::PacketType_ID_WELCOME: {
            if (handleWelcome(packet)) {
                m_welcomeReceived = true;
            }

            break;
        }

        case nier::PacketType_ID_SET_MASTER_CLIENT: {
            m_isMasterClient = true;
            break;
        }

        case nier::PacketType_ID_CREATE_PLAYER: {
            if (!handleCreatePlayer(packet)) {
                spdlog::error("Failed to create player");
            }

            break;
        }

        case nier::PacketType_ID_DESTROY_PLAYER: {
            if (!handleDestroyPlayer(packet)) {
                spdlog::error("Failed to destroy player");
            }

            break;
        }
        
        case nier::PacketType_ID_SPAWN_ENTITY: [[fallthrough]];
        case nier::PacketType_ID_DESTROY_ENTITY: [[fallthrough]];
        case nier::PacketType_ID_ENTITY_DATA: [[fallthrough]];
        case nier::PacketType_ID_ENTITY_ANIMATION_START: {
            const auto entityPacket = flatbuffers::GetRoot<nier::EntityPacket>(packet->data()->data());
            flatbuffers::Verifier entityVerif(packet->data()->data(), packet->data()->size());

            if (!entityPacket->Verify(entityVerif)) {
                spdlog::error("Invalid entity packet {} ({})", packet->id(), nier::EnumNamePacketType(packet->id()));
                return;
            }

            onEntityPacketReceived(packet->id(), entityPacket);
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

void NierClient::onPlayerPacketReceived(nier::PacketType packetType, const nier::PlayerPacket* packet) {
    spdlog::info("Player packet {} received from {}", nier::EnumNamePacketType(packetType), packet->guid());

    switch (packetType) {
        case nier::PacketType_ID_PLAYER_DATA: {
            if (!handlePlayerData(packet)) {
                spdlog::error("Failed to handle player data");
            }

            break;
        }
        case nier::PacketType_ID_ANIMATION_START: {
            if (!handleAnimationStart(packet)) {
                spdlog::error("Failed to handle animation start");
            }

            break;
        }
        case nier::PacketType_ID_BUTTONS: {
            if (!handleButtons(packet)) {
                spdlog::error("Failed to handle buttons");
            }

            break;
        }
        default:
            spdlog::error("Unknown player packet type {} ({})", packetType, nier::EnumNamePacketType(packetType));
            break;
    }
}

void NierClient::onEntityPacketReceived(nier::PacketType packetType, const nier::EntityPacket* packet) {
    spdlog::info("Entity packet {} received from {}", nier::EnumNamePacketType(packetType), packet->guid());

    switch (packetType) {
        case nier::PacketType_ID_SPAWN_ENTITY: {
            if (!handleCreateEntity(packet)) {
                spdlog::error("Failed to handle spawn entity");
            }

            break;
        }
        case nier::PacketType_ID_DESTROY_ENTITY: {
            if (!handleDestroyEntity(packet)) {
                spdlog::error("Failed to handle destroy entity");
            }

            break;
        }
        case nier::PacketType_ID_ENTITY_DATA: {
            if (!handleEntityData(packet)) {
                spdlog::error("Failed to handle entity data");
            }

            break;
        }
        case nier::PacketType_ID_ENTITY_ANIMATION_START: {
            if (!handleEntityAnimationStart(packet)) {
                spdlog::error("Failed to handle entity animation start");
            }

            break;
        }
        default:
            spdlog::error("Unknown entity packet type {} ({})", packetType, nier::EnumNamePacketType(packetType));
            break;
    }
}

void NierClient::sendPacket(nier::PacketType id, const uint8_t* data, size_t size) {
    auto builder = flatbuffers::FlatBufferBuilder{};

    uint32_t dataoffs = 0;

    if (data != nullptr && size > 0) {
        builder.StartVector(size, 1); // byte vector
        for (int64_t i = (int64_t)size - 1; i >= 0; i--) {
            builder.PushElement(data[i]);
        }
        dataoffs = builder.EndVector(size);
    }

    auto packetBuilder = nier::PacketBuilder(builder);

    packetBuilder.add_magic(1347240270);
    packetBuilder.add_id(id);

    if (data != nullptr && size > 0) {
        packetBuilder.add_data(dataoffs);
    }

    builder.Finish(packetBuilder.Finish());

    this->send_packet(0, builder.GetBufferPointer(), builder.GetSize(), ENET_PACKET_FLAG_RELIABLE);
}

void NierClient::sendAnimationStart(uint32_t anim, uint32_t variant, uint32_t a3, uint32_t a4) {
    nier::AnimationStart data{anim, variant, a3, a4};

    flatbuffers::FlatBufferBuilder builder(0);
    auto dataoffs = builder.CreateStruct(data);
    builder.Finish(dataoffs);

    sendPacket(nier::PacketType_ID_ANIMATION_START, builder.GetBufferPointer(), builder.GetSize());
}

void NierClient::sendButtons(const uint32_t* buttons) {
    flatbuffers::FlatBufferBuilder builder(0);
    const auto dataoffs = builder.CreateVector(buttons, sdk::Pl0000::EButtonIndex::INDEX_MAX);

    nier::Buttons::Builder dataBuilder(builder);
    dataBuilder.add_buttons(dataoffs);
    builder.Finish(dataBuilder.Finish());

    sendPacket(nier::PacketType_ID_BUTTONS, builder.GetBufferPointer(), builder.GetSize());
}

void NierClient::sendEntityPacket(nier::PacketType id, uint32_t guid, const uint8_t* data, size_t size) {
    flatbuffers::FlatBufferBuilder builder(0);
    const auto dataoffs = builder.CreateVector(data, size);

    nier::EntityPacket::Builder dataBuilder(builder);
    dataBuilder.add_guid(guid);
    dataBuilder.add_data(dataoffs);
    builder.Finish(dataBuilder.Finish());

    sendPacket(id, builder.GetBufferPointer(), builder.GetSize());
}

void NierClient::sendEntityCreate(uint32_t guid, sdk::EntitySpawnParams* data) {
    if (!m_isMasterClient) {
        spdlog::info("Not master client, not sending entity create");
        return;
    }

    // entity packet.
    flatbuffers::FlatBufferBuilder builder(0);
    const auto name = builder.CreateString(data->name);

    nier::EntitySpawnParams::Builder dataBuilder(builder);
    dataBuilder.add_name(name);
    dataBuilder.add_model(data->model);
    dataBuilder.add_model2(data->model2);

    if (data->matrix != nullptr) {
        dataBuilder.add_positional((nier::EntitySpawnPositionalData*)data->matrix);
    }

    builder.Finish(dataBuilder.Finish());

    sendEntityPacket(nier::PacketType_ID_SPAWN_ENTITY, guid, builder.GetBufferPointer(), builder.GetSize());
}

void NierClient::sendEntityDestroy(uint32_t guid) {
    if (!m_isMasterClient) {
        spdlog::info("Not master client, not sending entity destroy");
        return;
    }

    sendEntityPacket(nier::PacketType_ID_DESTROY_ENTITY, guid);
}

void NierClient::sendEntityData(uint32_t guid, sdk::BehaviorAppBase* entity) {
    if (!m_isMasterClient) {
        spdlog::info("Not master client, not sending entity data");
        return;
    }

    flatbuffers::FlatBufferBuilder builder(0);
    nier::EntityData newData(
        entity->facing(),
        0.0f, // entity is not a player.
        entity->health(),
        *(nier::Vector3f*)&entity->position()
    );

    builder.Finish(builder.CreateStruct(newData));

    m_networkEntities->processEntityData(guid, &newData);
    sendEntityPacket(nier::PacketType_ID_ENTITY_DATA, guid, builder.GetBufferPointer(), builder.GetSize());
}

void NierClient::sendEntityAnimationStart(uint32_t guid, uint32_t anim, uint32_t variant, uint32_t a3, uint32_t a4) {
    if (!m_isMasterClient) {
        spdlog::info("Not master client, not sending entity animation start");
        return;
    }
    
    flatbuffers::FlatBufferBuilder builder(0);
    nier::AnimationStart data{anim, variant, a3, a4};
    builder.Finish(builder.CreateStruct(data));

    sendEntityPacket(nier::PacketType_ID_ENTITY_ANIMATION_START, guid, builder.GetBufferPointer(), builder.GetSize());
}

void NierClient::onEntityCreated(sdk::Entity* entity, sdk::EntitySpawnParams* data) {
    if (!m_isMasterClient) {
        entity->behavior->terminate(); // destroy the entity. only the server or the master client should create entities.
        return;
    }

    m_networkEntities->onEntityCreated(entity, data);
}

void NierClient::onEntityDeleted(sdk::Entity* entity) {
    m_networkEntities->onEntityDeleted(entity);
}

void NierClient::sendHello() {
    auto ents = sdk::EntityList::get();
    auto possessed = ents->getPossessedEntity();

    if (possessed == nullptr || possessed->behavior == nullptr) {
        spdlog::error("No possessed entity");
        return;
    }
    

    flatbuffers::FlatBufferBuilder builder{};
    const auto name_pkt = builder.CreateString(m_helloName);
    const auto pwd_pkt = builder.CreateString(m_password);

    nier::HelloBuilder helloBuilder(builder);
    helloBuilder.add_major(nier::VersionMajor_Value);
    helloBuilder.add_minor(nier::VersionMinor_Value);
    helloBuilder.add_patch(nier::VersionPatch_Value);
    helloBuilder.add_name(name_pkt);
    helloBuilder.add_password(pwd_pkt);
    helloBuilder.add_model(possessed->behavior->model_index());

    builder.Finish(helloBuilder.Finish());

    sendPacket(nier::PacketType_ID_HELLO, builder.GetBufferPointer(), builder.GetSize());
    m_helloSent = true;
}

void NierClient::updateLocalPlayerData() {
    if (!m_helloSent || !m_welcomeReceived || m_guid == 0) {
        return;
    }

    auto it = m_players.find(m_guid);

    if (it == m_players.end() || it->second == nullptr) {
        spdlog::error("Local player not set up");
        return;
    }

    auto entityList = sdk::EntityList::get();

    if (entityList == nullptr) {
        return;
    }

    auto player = entityList->getPossessedEntity();

    if (player == nullptr) {
        return;
    }

    it->second->setHandle(player->handle);
}

void NierClient::sendPlayerData() {
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
    
    auto entity = player->getEntity();

    if (entity == nullptr) {
        spdlog::error("Cannot send player data without entity");
        return;
    }

    nier::PlayerData playerData(
        entity->flashlight(),
        entity->speed(),
        entity->facing(),
        entity->facing2(),
        entity->weapon_index(),
        entity->pod_index(),
        entity->character_controller().held_flags,
        *(nier::Vector3f*)&entity->position()
    );

    flatbuffers::FlatBufferBuilder builder{};
    const auto offs = builder.CreateStruct(playerData);
    builder.Finish(offs);

    sendPacket(nier::PacketType_ID_PLAYER_DATA, builder.GetBufferPointer(), builder.GetSize());
}

bool NierClient::handleWelcome(const nier::Packet* packet) {
    spdlog::info("Welcome packet received");

    const auto welcome = flatbuffers::GetRoot<nier::Welcome>(packet->data()->data());
    auto verif = flatbuffers::Verifier(packet->data()->data(), packet->data()->size());

    if (!welcome->Verify(verif)) {
        spdlog::error("Invalid welcome packet");
        return false;
    }

    m_isMasterClient = welcome->isMasterClient();
    m_guid = welcome->guid();
    const auto highestGuid = welcome->highestEntityGuid();

    spdlog::info("Welcome packet received, isMasterClient: {}, guid: {}", m_isMasterClient, m_guid);

    m_networkEntities = std::make_unique<EntitySync>(highestGuid);
    m_networkEntities->onEnterServer(m_isMasterClient);

    return true;
}

bool NierClient::handleCreatePlayer(const nier::Packet* packet) {
    spdlog::info("Create player packet received");

    auto entityList = sdk::EntityList::get();

    if (entityList == nullptr) {
        spdlog::error("Entity list not found while handling create player packet");
        return false;
    }

    auto possessed = entityList->getPossessedEntity();

    if (possessed == nullptr) {
        spdlog::error("Possessed entity not found while handling create player packet");
        return false;
    }

    auto localplayer = entityList->getByName("Player");

    if (localplayer == nullptr || localplayer->behavior == nullptr) {
        spdlog::info("Player not found while handling create player packet");
        return false;
    }

    const auto createPlayer = flatbuffers::GetRoot<nier::CreatePlayer>(packet->data()->data());
    auto verif = flatbuffers::Verifier(packet->data()->data(), packet->data()->size());

    if (!createPlayer->Verify(verif)) {
        spdlog::error("Invalid create player packet");
        return false;
    }

    {
        std::scoped_lock _{m_playersMtx};

        auto newPlayer = std::make_unique<Player>();
        newPlayer->setGuid(createPlayer->guid());
        newPlayer->setName(createPlayer->name()->c_str());

        m_players[createPlayer->guid()] = std::move(newPlayer);
    }

    // we don't want to spawn ourselves
    if (createPlayer->guid() != m_guid) {
        spdlog::info("Spawning player {}, {}", createPlayer->guid(), createPlayer->name()->c_str());

        MidHooks::s_ignoreSpawn = true;
        auto ent = entityList->spawnEntity("partner", createPlayer->model(), possessed->behavior->position());
        MidHooks::s_ignoreSpawn = false;

        if (ent != nullptr) {
            std::scoped_lock _{m_playersMtx};

            spdlog::info(" Player spawned");

            ent->behavior->as<sdk::Pl0000>()->buddy_handle() = localplayer->handle;
            localplayer->behavior->as<sdk::Pl0000>()->buddy_handle() = ent->handle;

            ent->behavior->setSuspend(false);

            ent->assignAIRoutine("PLAYER");
            ent->assignAIRoutine("player");

            // alternate way of assigning AI/control to the entity easily.
            localplayer->behavior->as<sdk::Pl0000>()->changePlayer();
            localplayer->behavior->as<sdk::Pl0000>()->changePlayer();

            ent->behavior->obj_flags() = -1;
            ent->behavior->as<sdk::Pl0000>()->setBuddyFromNpc();
            ent->behavior->obj_flags() = 0;

            m_players[createPlayer->guid()]->setStartTick(ent->behavior->tick_count());
            m_players[createPlayer->guid()]->setHandle(ent->handle);

            spdlog::info(" player assigned handle {:x}", ent->handle);
        } else {
            spdlog::error("Failed to spawn partner");
        }
    } else {
        spdlog::info("not spawning self");
    }

    return true;
}

bool NierClient::handleDestroyPlayer(const nier::Packet* packet) {
    spdlog::info("Destroy player packet received");

    const auto destroyPlayer = flatbuffers::GetRoot<nier::DestroyPlayer>(packet->data()->data());

    std::scoped_lock _{m_playersMtx};

    if (m_players.contains(destroyPlayer->guid()) && m_players[destroyPlayer->guid()] != nullptr) {
        auto entityList = sdk::EntityList::get();

        if (entityList == nullptr) {
            // not an error, we just won't actually delete any entity from the entity list
            spdlog::info("Entity list not found while handling destroy player packet");
        } else {
            auto localplayer = entityList->getByName("Player");
            auto ent = entityList->getByHandle(m_players[destroyPlayer->guid()]->getHandle());
            if (ent != nullptr && ent != localplayer) {
                ent->behavior->terminate();
            }
        }
    }

    m_players[destroyPlayer->guid()].reset();
    m_players.erase(destroyPlayer->guid());

    return true;
}

bool NierClient::handleCreateEntity(const nier::EntityPacket* packet) {
    spdlog::info("Create entity packet received");

    const auto spawn = flatbuffers::GetRoot<nier::EntitySpawnParams>(packet->data()->data());
    auto verif = flatbuffers::Verifier(packet->data()->data(), packet->data()->size());

    if (!spawn->Verify(verif)) {
        spdlog::error("Invalid create entity packet");
        return false;
    }

    auto entityList = sdk::EntityList::get();

    if (entityList != nullptr) {
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
        MidHooks::s_ignoreSpawn = true;
        auto ent = entityList->spawnEntity(params);
        MidHooks::s_ignoreSpawn = false;

        if (ent != nullptr) {
            //ent->entity->setSuspend(false);

            spdlog::info(" Entity spawned @ {:x}", (uintptr_t)ent);
            auto newNetworkEnt = m_networkEntities->addEntity(ent, packet->guid());

            if (newNetworkEnt != nullptr) {
                spdlog::info(" Network entity created");
            }
        } else {
            spdlog::error(" Failed to spawn entity");
        }
    }

    return true;
}

bool NierClient::handleDestroyEntity(const nier::EntityPacket* packet) {
    spdlog::info("Destroy entity packet received");

    m_networkEntities->removeEntity(packet->guid());

    return true;
}

bool NierClient::handleEntityData(const nier::EntityPacket* packet) {
    spdlog::info("Entity data packet received");

    const auto entityData = flatbuffers::GetRoot<nier::EntityData>(packet->data()->data());
    m_networkEntities->processEntityData(packet->guid(), entityData);

    return true;
}

bool NierClient::handleEntityAnimationStart(const nier::EntityPacket* packet) {
    spdlog::info("Entity animation start packet received");

    const auto guid = packet->guid();
    auto entityNetworked = m_networkEntities->getNetworkEntityFromGuid(guid);

    if (entityNetworked == nullptr) {
        spdlog::error(" (nullptr) Entity data packet received for unknown entity {}", guid);
        return false;
    }

    auto animationData = flatbuffers::GetRoot<nier::AnimationStart>(packet->data()->data());
    auto npc = entityNetworked->getEntity() != nullptr ? entityNetworked->getEntity()->behavior : nullptr;

    if (npc != nullptr) {
        switch (animationData->anim()) {
        case sdk::EAnimation::INVALID_CRASHES_GAME:
        case sdk::EAnimation::INVALID_CRASHES_GAME2:
        case sdk::EAnimation::INVALID_CRASHES_GAME3:
        case sdk::EAnimation::INVALID_CRASHES_GAME4:
            return true;
        default:
            if (npc) {
                npc->start_animation(animationData->anim(), animationData->variant(), animationData->a3(), animationData->a4());
            } else {
                spdlog::error(" Cannot start animation, npc is null");
            }
        }
    }

    return true;
}

bool NierClient::handlePlayerData(const nier::PlayerPacket* packet) {
    const auto guid = packet->guid();

    // do not update the local player. maybe change this later for forced updates/teleportation commands?
    if (guid == m_guid) {
        return true;
    }

    if (!m_players.contains(guid)) {
        spdlog::error("Player data packet received for unknown player {}", guid);
        return false;
    }

    const auto& playerNetworked = m_players[guid];

    if (playerNetworked == nullptr) {
        spdlog::error("(nullptr) Player data packet received for unknown player {}", guid);
        return false;
    }

    auto playerData = flatbuffers::GetRoot<nier::PlayerData>(packet->data()->data());
    auto npc = playerNetworked->getEntity();
    
    if (npc != nullptr) {
        npc->position() = *(Vector3f*)&playerData->position();
    }

    playerNetworked->setPlayerData(*playerData);

    return true;
}

bool NierClient::handleAnimationStart(const nier::PlayerPacket* packet) {
    const auto guid = packet->guid();

    // do not update the local player. maybe change this later for forced updates/teleportation commands?
    if (guid == m_guid) {
        return true;
    }

    if (!m_players.contains(guid)) {
        spdlog::error("Player data packet received for unknown player {}", guid);
        return false;
    }

    const auto& playerNetworked = m_players[guid];

    if (playerNetworked == nullptr) {
        spdlog::error("(nullptr) Player data packet received for unknown player {}", guid);
        return false;
    }

    auto animationData = flatbuffers::GetRoot<nier::AnimationStart>(packet->data()->data());
    auto npc = playerNetworked->getEntity();

    if (npc != nullptr) {
        switch (animationData->anim()) {
        case sdk::EAnimation::INVALID_CRASHES_GAME:
        case sdk::EAnimation::INVALID_CRASHES_GAME2:
        case sdk::EAnimation::INVALID_CRASHES_GAME3:
        case sdk::EAnimation::INVALID_CRASHES_GAME4:
        case sdk::EAnimation::Light_Attack:
            return true;
        default:
            if (npc) {
                npc->start_animation(animationData->anim(), animationData->variant(), animationData->a3(), animationData->a4());
            } else {
                spdlog::error("Cannot start animation, npc is null");
            }
        }
    }
    
    return true;
}

bool NierClient::handleButtons(const nier::PlayerPacket* packet) {
    const auto guid = packet->guid();
    
    // do not update the local player. maybe change this later for forced updates/teleportation commands?
    if (guid == m_guid) {
        return true;
    }

    if (!m_players.contains(guid)) {
        spdlog::error("Player data packet received for unknown player {}", guid);
        return false;
    }

    const auto& playerNetworked = m_players[guid];

    if (playerNetworked == nullptr) {
        spdlog::error("(nullptr) Player data packet received for unknown player {}", guid);
        return false;
    }

    auto buttons = flatbuffers::GetRoot<nier::Buttons>(packet->data()->data());
    auto npc = playerNetworked->getEntity();

    if (npc != nullptr) {
        const auto buttonsData = buttons->buttons()->data();
        const auto sizeButtons = sizeof(regenny::CharacterController::buttons);
        memcpy(&npc->character_controller().buttons, buttonsData, sizeButtons);

        for (uint32_t i = 0; i < sdk::Pl0000::EButtonIndex::INDEX_MAX; ++i) {
            auto& controller = npc->character_controller();

            if (buttonsData[i] > 0) {
                controller.held_flags |= (1 << i);
            }
        }
    }

    return true;
}
