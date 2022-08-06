#pragma once

#include <unordered_map>
#include <mutex>

#include "schema/Packets_generated.h"
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

    void setEntityData(const nier::EntityData& data) {
        m_entityData = data;
    }

    auto getGuid() const {
        return m_guid;
    }

    void setGuid(uint32_t guid) {
        m_guid = guid;
    }

private:
    uint32_t m_guid{};
    EntityContainer* m_entity;
    nier::EntityData m_entityData;
};

class EntitySync {
public:
    void onEntityCreated(EntityContainer* entity, EntitySpawnParams* data);
    void onEntityDeleted(EntityContainer* entity);

    std::shared_ptr<NetworkEntity> addEntity(EntityContainer* entity, uint32_t guid);
    void removeEntity(uint32_t identifier);

    void think();
    void processEntityData(uint32_t guid, const nier::EntityData* data);

    std::shared_ptr<NetworkEntity> getNetworkEntityFromHandle(uint32_t handle) {
        auto it = m_handleMap.find(handle);

        if (it == m_handleMap.end()) {
            return nullptr;
        }

        auto it2 = m_networkEntities.find(it->second);

        if (it2 == m_networkEntities.end()) {
            return nullptr;
        }

        return it2->second;
    }

private:
    uint32_t m_maxGuid{ 0 };
    std::unordered_map<uint32_t, std::shared_ptr<NetworkEntity>> m_networkEntities;
    std::unordered_map<uint32_t, uint32_t> m_handleMap;
    std::recursive_mutex m_mapMutex;
};