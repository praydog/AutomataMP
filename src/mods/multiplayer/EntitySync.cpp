#include <spdlog/spdlog.h>

#include "schema/Packets_generated.h"

#include "mods/AutomataMPMod.hpp"
#include "EntitySync.hpp"

using namespace std;

EntitySync* g_entitySync = nullptr;

NetworkEntity::NetworkEntity(EntityContainer* entity, uint32_t guid) 
    : m_guid(guid)
    , m_entityHandle(entity->handle)
{
    scoped_lock _(g_entitySync->m_mapMutex);

    spdlog::info("Hooking entity {}", guid);
    m_hook = std::make_unique<VtableHook>();
    
    if (m_hook->create(entity->entity)) {
        m_hook->hookMethod(Entity::s_startAnimationIndex, &startAnimationHook);
        spdlog::info("Hooked entity {}", guid);
    }
}

void NetworkEntity::startAnimationHook(Entity* ent, uint32_t anim, uint32_t variant, uint32_t a3, uint32_t a4) {
    scoped_lock _(g_entitySync->m_mapMutex);

    spdlog::info("NETWORKENTITY anim: {}, variant: {}, a3: {}, return: {:x}", anim, variant, a3, (uintptr_t)_ReturnAddress());

    auto amp = AutomataMPMod::get();
    auto& client = amp->getClient();

    auto networkEntity = g_entitySync->getNetworkEntityFromHandle(ent->getContainer()->handle);

    if (client != nullptr) {
        client->sendEntityAnimationStart(networkEntity->getGuid(), anim, variant, a3, a4);
    }

    auto original = networkEntity->m_hook->getMethod<decltype(startAnimationHook)*>(Entity::s_startAnimationIndex);
    original(ent, anim, variant, a3, a4);
}

EntitySync::EntitySync() {
    g_entitySync = this;
}

void EntitySync::onEntityCreated(EntityContainer* entity, EntitySpawnParams* data) {
    scoped_lock _(m_mapMutex);

    auto guid = m_maxGuid++;

    addEntity(entity, guid);

    AutomataMPMod::get()->getClient()->sendEntityCreate(guid, data);

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

void EntitySync::onEntityDeleted(EntityContainer* entity) {
    scoped_lock _(m_mapMutex);

    auto networkedEntity = getNetworkEntityFromHandle(entity->handle);

    if (networkedEntity == nullptr) {
        return;
    }

    m_handleMap.erase(entity->handle);
    removeEntity(networkedEntity->getGuid());

    AutomataMPMod::get()->getClient()->sendEntityDestroy(networkedEntity->getGuid());
}

std::shared_ptr<NetworkEntity> EntitySync::addEntity(EntityContainer* entity, uint32_t guid) {
    spdlog::info("Adding entity {:x} with guid {}", (uintptr_t)entity, guid);

    scoped_lock _(m_mapMutex);
    auto& networkEntity = m_networkEntities[guid];

    if (networkEntity == nullptr || networkEntity->getEntity() != entity) {
        networkEntity.reset();
        networkEntity = std::make_shared<NetworkEntity>(entity, guid);
    }

    m_handleMap[entity->handle] = guid;
    networkEntity->setGuid(guid);
    networkEntity->setEntity(entity);

    nier::EntityData firstData(
        *entity->entity->getFacing(),
        *entity->entity->getFacing2(),
        *entity->entity->getHealth(),
        *(nier::Vector3f*)&*entity->entity->getPosition());

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
        auto npc = ent->entity;

        if (npc == nullptr) {
            continue;
        }

        if (AutomataMPMod::get()->isServer()) {
            /*packet.position = *npc->getPosition();
            packet.facing = *npc->getFacing();
            packet.facing2 = *npc->getFacing2();
            packet.health = *npc->getHealth();*/

            AutomataMPMod::get()->getClient()->sendEntityData(it.first, npc);
        }
        else {
            *npc->getPosition() = *(Vector3f*)&packet.position();
            *npc->getFacing() = packet.facing();
            *npc->getFacing2() = packet.facing2();
            *npc->getHealth() = packet.health();
        }

        npc->setSuspend(false);
    }

    // genius moment
    try {
        const auto isMasterClient = AutomataMPMod::get()->isServer();

        // Delete any entities that are not supposed to be networked.
        if (!isMasterClient) {
            auto entityList = EntityList::get();

            if (entityList == nullptr) {
                return;
            }

            for (auto i = 0; i < entityList->size(); ++i) {
                auto container = entityList->get(i);

                if (container == nullptr) {
                    continue;
                }

                auto entity = container->entity;

                if (entity == nullptr) {
                    continue;
                }

                // Delete any entities that are not supposed to be networked.
                if (!m_handleMap.contains(container->handle) && entity->isNetworkable()) {
                    spdlog::info("Deleting entity {:x} {}", (uintptr_t)container, container->name);
                    entity->terminate();
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

        auto npc = cont->entity;

        if (npc == nullptr) {
            return;
        }

        *npc->getPosition() = *(Vector3f*)&data->position();
        *npc->getFacing() = data->facing();
        *npc->getFacing2() = data->facing2();
        *npc->getHealth() = data->health();

        ent->setEntityData(*data);
    }
}
