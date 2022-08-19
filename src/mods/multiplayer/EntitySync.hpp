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
    NetworkEntity(sdk::Entity* entity, uint32_t guid);

    void set_entity(sdk::Entity* entity) { m_entity_handle = entity->handle; }

    sdk::Entity* get_entity() {
        auto entity_list = sdk::EntityList::get();

        if (entity_list == nullptr) {
            return nullptr;
        }

        return entity_list->get_by_handle(m_entity_handle);
    }

    auto& get_entity_data() { return m_entity_data; }

    void set_entity_data(const nier::EntityData& data) { m_entity_data = data; }

    auto get_guid() const { return m_guid; }

    void set_guid(uint32_t guid) { m_guid = guid; }

private:
    friend class EntitySync;
    static void start_animation_hook(sdk::Behavior* ent, uint32_t anim, uint32_t variant, uint32_t a3, uint32_t a4);

    std::unique_ptr<VtableHook> m_hook{};
    uint32_t m_guid{};
    uint32_t m_entity_handle{};
    nier::EntityData m_entity_data;
};

class EntitySync {
public:
    EntitySync(uint32_t highest_guid = 0);

    void on_entity_created(sdk::Entity* entity, sdk::EntitySpawnParams* data);
    void on_entity_deleted(sdk::Entity* entity);
    void on_enter_server(bool is_master_client); // Gather existing entities (if master client) and send them to server

    std::shared_ptr<NetworkEntity> add_entity(sdk::Entity* entity, uint32_t guid);
    void remove_entity(uint32_t identifier);

    void think();
    void process_entity_data(uint32_t guid, const nier::EntityData* data);

    std::shared_ptr<NetworkEntity> get_network_entity_from_handle(uint32_t handle) {
        auto it = m_handle_map.find(handle);

        if (it == m_handle_map.end()) {
            return nullptr;
        }

        auto it2 = m_network_entities.find(it->second);

        if (it2 == m_network_entities.end()) {
            return nullptr;
        }

        return it2->second;
    }

    std::shared_ptr<NetworkEntity> get_network_entity_from_guid(uint32_t guid) {
        auto it = m_network_entities.find(guid);

        if (it == m_network_entities.end()) {
            return nullptr;
        }

        return it->second;
    }

private:
    friend class NetworkEntity;
    uint32_t m_max_guid{0};
    std::unordered_map<uint32_t, std::shared_ptr<NetworkEntity>> m_network_entities;
    std::unordered_map<uint32_t, uint32_t> m_handle_map;
    std::recursive_mutex m_map_mutex;
};