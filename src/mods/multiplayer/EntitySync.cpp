#include <spdlog/spdlog.h>

#include "schema/Packets_generated.h"

#include "mods/AutomataMPMod.hpp"
#include "EntitySync.hpp"

using namespace std;

EntitySync* g_entitySync = nullptr;

NetworkEntity::NetworkEntity(sdk::Entity* entity, uint32_t guid) 
    : m_guid(guid)
    , m_entityHandle(entity->handle)
{
    scoped_lock _(g_entitySync->m_mapMutex);

    spdlog::info("Hooking entity {}", guid);
    m_hook = std::make_unique<VtableHook>();
    
    if (m_hook->create(entity->behavior)) {
        m_hook->hookMethod(sdk::Behavior::s_start_animation_index, &startAnimationHook);
        spdlog::info("Hooked entity {}", guid);
    }
}

void NetworkEntity::startAnimationHook(sdk::Behavior* behavior, uint32_t anim, uint32_t variant, uint32_t a3, uint32_t a4) {
    scoped_lock _(g_entitySync->m_mapMutex);

    spdlog::info("NETWORKENTITY anim: {}, variant: {}, a3: {}, return: {:x}", anim, variant, a3, (uintptr_t)_ReturnAddress());

    auto amp = AutomataMPMod::get();
    auto& client = amp->get_client();

    auto networkEntity = g_entitySync->getNetworkEntityFromHandle(behavior->get_entity()->handle);

    if (client != nullptr) {
        client->send_entity_animation_start(networkEntity->getGuid(), anim, variant, a3, a4);
    }

    auto original = networkEntity->m_hook->getMethod<decltype(startAnimationHook)*>(sdk::Behavior::s_start_animation_index);
    original(behavior, anim, variant, a3, a4);
}

EntitySync::EntitySync(uint32_t highestGuid)
    : m_maxGuid{highestGuid}
{
    g_entitySync = this;
}

void EntitySync::onEntityCreated(sdk::Entity* entity, sdk::EntitySpawnParams* data) {
    scoped_lock _(m_mapMutex);

    auto guid = m_maxGuid++;

    addEntity(entity, guid);

    AutomataMPMod::get()->get_client()->send_entity_create(guid, data);

    /*nier_server::EntitySpawn packet;
    packet.guid = guid;
    packet.model = data->model;
    packet.model2 = data->model2;

    if (data->name) {
        strcpy_s(packet.name, data->name);
    }

    if (data->matrix) {
        packet.matrix = *data->matrix;
    }

    spdlog::info("Sending enemy spawn {:x} with guid {}", (uintptr_t)entity, packet.guid);

    AutomataMPMod::get()->sendPacket(packet.data(), sizeof(packet));*/
}

void EntitySync::onEntityDeleted(sdk::Entity* entity) {
    scoped_lock _(m_mapMutex);

    auto networkedEntity = getNetworkEntityFromHandle(entity->handle);

    if (networkedEntity == nullptr) {
        return;
    }

    m_handleMap.erase(entity->handle);
    removeEntity(networkedEntity->getGuid());

    AutomataMPMod::get()->get_client()->send_entity_destroy(networkedEntity->getGuid());
}

void EntitySync::onEnterServer(bool is_master_client) try {
    scoped_lock _(m_mapMutex);

    if (is_master_client) {
        auto entityList = sdk::EntityList::get();

        if (entityList == nullptr) {
            return;
        }

        for (auto i = 0; i < entityList->size(); ++i) {
            auto container = entityList->get(i);

            if (container == nullptr) {
                continue;
            }

            auto behavior = container->behavior;

            if (behavior == nullptr) {
                continue;
            }

            // Send any existing valid entities
            // that are not currently networked to the server.
            if (!m_handleMap.contains(container->handle) && behavior->is_networkable()) {
                spdlog::info("Sending existing entity {}", container->name);

                sdk::EntitySpawnParams spawnParams{};
                sdk::EntitySpawnParams::PositionalData positionalData{};
                spawnParams.name = container->name;
                spawnParams.model = behavior->model_index();
                spawnParams.model2 = behavior->model_index();

                positionalData.position = Vector4f{behavior->position(), 1.0f};
                spawnParams.matrix = &positionalData;

                onEntityCreated(container, &spawnParams);
            }
        }
    }
} catch(...) {

}

std::shared_ptr<NetworkEntity> EntitySync::addEntity(sdk::Entity* entity, uint32_t guid) {
    spdlog::info("Adding entity {:x} with guid {}", (uintptr_t)entity, guid);

    scoped_lock _(m_mapMutex);

    // This allows someone taking over as master client
    // to not screw up any previously spawned entities.
    if (guid > m_maxGuid) {
        m_maxGuid = guid;
    }

    auto& networkEntity = m_networkEntities[guid];

    if (networkEntity == nullptr || networkEntity->getEntity() != entity) {
        networkEntity.reset();
        networkEntity = std::make_shared<NetworkEntity>(entity, guid);
    }

    m_handleMap[entity->handle] = guid;
    networkEntity->setGuid(guid);
    networkEntity->setEntity(entity);

    nier::EntityData firstData(
        entity->behavior->facing(),
        //entity->behavior->as<sdk::Pl0000>()->character_controller().facing,
        0.0f,
        entity->behavior->as<sdk::BehaviorAppBase>()->health(),
        *(nier::Vector3f*)&entity->behavior->position()
    );

    networkEntity->setEntityData(firstData);

    return networkEntity;
}

void EntitySync::removeEntity(uint32_t identifier) {
    scoped_lock _(m_mapMutex);

    if (m_networkEntities.find(identifier) != m_networkEntities.end()) {
        spdlog::info("Removing entity from EntitySync");
        m_networkEntities.erase(identifier);
    }
}

void EntitySync::think() {
    scoped_lock _(m_mapMutex);

    for (auto& it : m_networkEntities) {
        auto networkedEntity = it.second;
        auto ent = networkedEntity->getEntity();

        if (ent == nullptr) {
            continue;
        }

        auto& packet = networkedEntity->getEntityData();
        auto npc = ent->behavior->as<sdk::BehaviorAppBase>();

        if (npc == nullptr) {
            continue;
        }

        if (AutomataMPMod::get()->is_server()) {
            /*packet.position = *npc->getPosition();
            packet.facing = *npc->getFacing();
            packet.facing2 = *npc->getFacing2();
            packet.health = *npc->getHealth();*/

            AutomataMPMod::get()->get_client()->send_entity_data(it.first, npc);
        }
        else {
            npc->position() = *(Vector3f*)&packet.position();
            npc->facing() = packet.facing();
            //npc->getFacing2() = packet.facing2();
            npc->health() = packet.health();
        }

        npc->setSuspend(false);
    }

    // genius moment
    try {
        const auto is_master_client = AutomataMPMod::get()->is_server();

        // Delete any entities that are not supposed to be networked.
        if (!is_master_client) {
            auto entityList = sdk::EntityList::get();

            if (entityList == nullptr) {
                return;
            }

            for (auto i = 0; i < entityList->size(); ++i) {
                auto container = entityList->get(i);

                if (container == nullptr) {
                    continue;
                }

                auto behavior = container->behavior;

                if (behavior == nullptr) {
                    continue;
                }

                // Delete any entities that are not supposed to be networked.
                if (!m_handleMap.contains(container->handle) && behavior->is_networkable()) {
                    spdlog::info("Deleting entity {:x} {}", (uintptr_t)container, container->name);
                    behavior->terminate();
                }
            }
        }
    } catch(...) {

    }
}

void EntitySync::processEntityData(uint32_t guid, const nier::EntityData* data) {
    //spdlog::info("Processing {} entity data", guid);

    scoped_lock _(m_mapMutex);
    if (auto it = m_networkEntities.find(guid); it != m_networkEntities.end()) {
        auto ent = it->second;

        const auto cont = ent->getEntity();

        if (cont == nullptr) {
            return;
        }

        auto npc = cont->behavior->as<sdk::BehaviorAppBase>();

        if (npc == nullptr) {
            return;
        }

        npc->position() = *(Vector3f*)&data->position();
        npc->facing() = data->facing();
        //*npc->getFacing2() = data->facing2();
        npc->health() = data->health();

        ent->setEntityData(*data);
    }
}
