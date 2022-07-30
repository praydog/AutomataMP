#pragma once

#include <unordered_map>
#include <mutex>

#include "Packets.hpp"
#include <sdk/Entity.hpp>
#include <sdk/EntityList.hpp>

class NetworkEntity {
public:
    void setEntity(EntityContainer* entity) {
        m_entity = entity;
    }

    auto getEntity() {
        return m_entity;
    }

    auto& getEntityData() {
        return m_entityData;
    }

    void setEntityData(nier_server::EntityData& data) {
        m_entityData = data;
    }

private:
    EntityContainer* m_entity;
    nier_server::EntityData m_entityData;
};

class EntitySync {
public:
    void onEntityCreated(EntityContainer* entity, EntitySpawnParams* data);
    void onEntityDeleted(EntityContainer* entity);

    NetworkEntity& addEntity(EntityContainer* entity, uint32_t guid);
    void removeEntity(uint32_t identifier);

    void think();
    void processEntityData(nier_server::EntityData* data);

private:
    uint32_t m_maxGuid{ 0 };
    std::unordered_map<uint32_t, NetworkEntity> m_networkEntities;
    std::unordered_map<uint32_t, NetworkEntity*> m_handleMap;
    std::recursive_mutex m_mapMutex;
};