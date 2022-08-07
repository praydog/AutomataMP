#pragma once

#include <unordered_map>
#include <mutex>

#include <utility/VtableHook.hpp>

#include "schema/Packets_generated.h"
#include <sdk/Entity.hpp>
#include <sdk/EntityList.hpp>

class EntitySync;

class NetworkEntity {
public:
    NetworkEntity(EntityContainer* entity, uint32_t guid);

    void setEntity(EntityContainer* entity) {
        m_entityHandle = entity->handle;
    }

    EntityContainer* getEntity() {
        auto entityList = EntityList::get();

        if (entityList == nullptr) {
            return nullptr;
        }

        return entityList->getByHandle(m_entityHandle);
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
    friend class EntitySync;
    static void startAnimationHook(Entity* ent, uint32_t anim, uint32_t variant, uint32_t a3, uint32_t a4);

    std::unique_ptr<VtableHook> m_hook{};
    uint32_t m_guid{};
    uint32_t m_entityHandle{};
    nier::EntityData m_entityData;
};

class EntitySync {
public:
    EntitySync();

    void onEntityCreated(EntityContainer* entity, EntitySpawnParams* data);
    void onEntityDeleted(EntityContainer* entity);
    void onEnterServer(bool isMasterClient); // Gather existing entities (if master client) and send them to server

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

    std::shared_ptr<NetworkEntity> getNetworkEntityFromGuid(uint32_t guid) {
        auto it = m_networkEntities.find(guid);

        if (it == m_networkEntities.end()) {
            return nullptr;
        }

        return it->second;
    }

private:
    friend class NetworkEntity;
    uint32_t m_maxGuid{ 0 };
    std::unordered_map<uint32_t, std::shared_ptr<NetworkEntity>> m_networkEntities;
    std::unordered_map<uint32_t, uint32_t> m_handleMap;
    std::recursive_mutex m_mapMutex;
};