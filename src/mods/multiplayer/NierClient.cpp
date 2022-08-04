#include <thread>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "schema/Packets_generated.h"
#include "Packets.hpp"
#include "mods/AutomataMPMod.hpp"
#include "NierClient.hpp"

using namespace std;

NierClient::NierClient(const std::string& host, const std::string& name, const std::string& password)
    : m_helloName{ name },
    m_password{ password }
{
    set_trace_handler([](const std::string& s) { spdlog::info("{}", s); });
    connect(enetpp::client_connect_params().set_channel_count(1).set_server_host_name_and_port(host.c_str(), 6969).set_timeout(chrono::seconds(1)));

    while (get_connection_state() == enetpp::CONNECT_CONNECTING) {
        think();
        this_thread::yield();
    }
}

NierClient::~NierClient() {
    disconnect();
}

void NierClient::think() {
    consume_events(
        [this]() { onConnect(); },
        [this]() { onDisconnect(); },
        [this](const enet_uint8* a, size_t b) { 
            onDataReceived(a, b); 
        }
    );
}

void NierClient::on_draw_ui() {
}

void NierClient::onConnect() {
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
    spdlog::info("Player packet {}", nier::EnumNamePacketType(packetType));

    switch (packetType) {
        case nier::PacketType_ID_PLAYER_DATA: {
            if (!handlePlayerData(packet)) {
                spdlog::error("Failed to handle player data");
            }

            break;
        }
        default:
            spdlog::error("Unknown player packet type {} ({})", packetType, nier::EnumNamePacketType(packetType));
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

void NierClient::sendHello() {
    flatbuffers::FlatBufferBuilder builder{};
    const auto name_pkt = builder.CreateString(m_helloName);
    const auto pwd_pkt = builder.CreateString(m_password);

    nier::HelloBuilder helloBuilder(builder);
    helloBuilder.add_major(nier::VersionMajor_Value);
    helloBuilder.add_minor(nier::VersionMinor_Value);
    helloBuilder.add_patch(nier::VersionPatch_Value);
    helloBuilder.add_name(name_pkt);
    helloBuilder.add_password(pwd_pkt);

    builder.Finish(helloBuilder.Finish());

    sendPacket(nier::PacketType_ID_HELLO, builder.GetBufferPointer(), builder.GetSize());
    m_helloSent = true;
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

    spdlog::info("Welcome packet received, isMasterClient: {}, guid: {}", m_isMasterClient, m_guid);

    return true;
}

bool NierClient::handleCreatePlayer(const nier::Packet* packet) {
    spdlog::info("Create player packet received");

    auto entityList = EntityList::get();

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

    if (localplayer == nullptr || localplayer->entity == nullptr) {
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
        auto newPlayer = std::make_unique<Player>();
        newPlayer->setGuid(createPlayer->guid());
        newPlayer->setName(createPlayer->name()->c_str());

        m_players[createPlayer->guid()] = std::move(newPlayer);
    }

    // we don't want to spawn ourselves
    if (createPlayer->guid() != m_guid) {
        spdlog::info("Spawning partner");

        auto ent = entityList->spawnEntity("partner", createPlayer->model(), *possessed->entity->getPosition());

        if (ent != nullptr) {
            spdlog::info("Partner spawned");

            ent->entity->setBuddyHandle(localplayer->handle);
            localplayer->entity->setBuddyHandle(ent->handle);

            ent->entity->setSuspend(false);

            ent->assignAIRoutine("PLAYER");
            ent->assignAIRoutine("player");

            // alternate way of assigning AI/control to the entity easily.
            //localplayer->entity->changePlayer();
            //localplayer->entity->changePlayer();

            const auto old_flags = ent->entity->getBuddyFlags();
            ent->entity->setBuddyFlags(-1);
            ent->entity->setBuddyFromNpc();
            ent->entity->setBuddyFlags(0);

            m_players[createPlayer->guid()]->setStartTick(*ent->entity->getTickCount());
            m_players[createPlayer->guid()]->setHandle(ent->handle);
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

    if (m_players.contains(destroyPlayer->guid()) && m_players[destroyPlayer->guid()] != nullptr) {
        auto entityList = EntityList::get();

        if (entityList == nullptr) {
            // not an error, we just won't actually delete any entity from the entity list
            spdlog::info("Entity list not found while handling destroy player packet");
        } else {
            auto localplayer = entityList->getByName("Player");
            auto ent = entityList->getByHandle(m_players[destroyPlayer->guid()]->getHandle());
            if (ent != nullptr && ent != localplayer) {
                ent->entity->terminate();
            }
        }
    }

    m_players[destroyPlayer->guid()].reset();
    m_players.erase(destroyPlayer->guid());

    return true;
}

bool NierClient::handlePlayerData(const nier::PlayerPacket* packet) {
    return true;
}
