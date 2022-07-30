#include <spdlog/spdlog.h>

#include "mods/AutomataMPMod.hpp"
#include "EntitySync.hpp"

using namespace std;

void EntitySync::onEntityCreated(EntityContainer* entity, EntitySpawnParams* data) {
    auto guid = m_maxGuid++;

    addEntity(entity, guid);

    nier_server::EntitySpawn packet;
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

    AutomataMPMod::get()->sendPacket(packet.data(), sizeof(packet));
}

void EntitySync::onEntityDeleted(EntityContainer* entity) {
    lock_guard<mutex> _(m_mapMutex);

    if (m_handleMap.find(entity->handle) == m_handleMap.end()) {
        return;
    }

    auto guid = m_handleMap[entity->handle]->getEntityData().guid;

    m_handleMap.erase(entity->handle);
    removeEntity(guid);
}

NetworkEntity& EntitySync::addEntity(EntityContainer* entity, uint32_t guid) {
    spdlog::info("Adding entity {:x} with guid {}", (uintptr_t)entity, guid);

    lock_guard<mutex> _(m_mapMutex);
    auto& networkEntity = m_networkEntities[guid];
    m_handleMap[entity->handle] = &networkEntity;
    networkEntity.getEntityData().guid = guid;
    networkEntity.setEntity(entity);

    return networkEntity;
}

void EntitySync::removeEntity(uint32_t identifier) {
    if (m_networkEntities.find(identifier) != m_networkEntities.end()) {
        spdlog::info("Removing entity from EntitySync");
        m_networkEntities.erase(identifier);
    }
}

void EntitySync::think() {
    lock_guard<mutex> _(m_mapMutex);

    for (auto& networkedEnt : m_networkEntities) {
        auto ent = networkedEnt.second.getEntity();
        auto& packet = networkedEnt.second.getEntityData();
        auto npc = ent->entity;

        if (AutomataMPMod::get()->isServer()) {
            packet.position = *npc->getPosition();
            packet.facing = *npc->getFacing();
            packet.facing2 = *npc->getFacing2();
            packet.health = *npc->getHealth();

            AutomataMPMod::get()->sendPacket(packet.data(), sizeof(packet));
        }
        else {
            *npc->getPosition() = packet.position;
            *npc->getFacing() = packet.facing;
            *npc->getFacing2() = packet.facing2;
            *npc->getHealth() = packet.health;
        }

        npc->setSuspend(false);
    }
}

void EntitySync::processEntityData(nier_server::EntityData* data) {
    spdlog::info("Processing {} entity data", data->guid);

    lock_guard<mutex> _(m_mapMutex);
    if (m_networkEntities.find(data->guid) != m_networkEntities.end()) {
        auto ent = m_networkEntities[data->guid];
        auto npc = ent.getEntity()->entity;

        *npc->getPosition() = data->position;
        *npc->getFacing() = data->facing;
        *npc->getFacing2() = data->facing2;
        *npc->getHealth() = data->health;

        ent.setEntityData(*data);
    }
}
