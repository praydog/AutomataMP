#include <spdlog/spdlog.h>

#include "schema/Packets_generated.h"

#include "mods/AutomataMPMod.hpp"
#include "EntitySync.hpp"

using namespace std;

EntitySync* g_entity_sync = nullptr;

NetworkEntity::NetworkEntity(sdk::Entity* entity, uint32_t guid)
    : m_guid(guid)
    , m_entity_handle(entity->handle) {
    scoped_lock _(g_entity_sync->m_map_mutex);

    spdlog::info("Hooking entity {}", guid);
    m_hook = std::make_unique<VtableHook>();
    
    if (m_hook->create(entity->behavior)) {
        m_hook->hook_method(sdk::Behavior::s_start_animation_index, &start_animation_hook);
        spdlog::info("Hooked entity {}", guid);
    }
}

void NetworkEntity::start_animation_hook(sdk::Behavior* behavior, uint32_t anim, uint32_t variant, uint32_t a3, uint32_t a4) {
    scoped_lock _(g_entity_sync->m_map_mutex);

    spdlog::info("NETWORKENTITY anim: {}, variant: {}, a3: {}, return: {:x}", anim, variant, a3, (uintptr_t)_ReturnAddress());

    auto amp = AutomataMPMod::get();
    auto& client = amp->get_client();

    auto network_entity = g_entity_sync->get_network_entity_from_handle(behavior->get_entity()->handle);

    if (client != nullptr) {
        client->send_entity_animation_start(network_entity->get_guid(), anim, variant, a3, a4);
    }

    auto original = network_entity->m_hook->get_method<decltype(start_animation_hook)*>(sdk::Behavior::s_start_animation_index);
    original(behavior, anim, variant, a3, a4);
}

EntitySync::EntitySync(uint32_t highest_guid)
    : m_max_guid{highest_guid} {
    g_entity_sync = this;
}

void EntitySync::on_entity_created(sdk::Entity* entity, sdk::EntitySpawnParams* data) {
    scoped_lock _(m_map_mutex);

    auto guid = m_max_guid++;

    add_entity(entity, guid);

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

void EntitySync::on_entity_deleted(sdk::Entity* entity) {
    scoped_lock _(m_map_mutex);

    auto networked_entity = get_network_entity_from_handle(entity->handle);

    if (networked_entity == nullptr) {
        return;
    }

    m_handle_map.erase(entity->handle);
    remove_entity(networked_entity->get_guid());

    AutomataMPMod::get()->get_client()->send_entity_destroy(networked_entity->get_guid());
}

void EntitySync::on_enter_server(bool is_master_client) try {
    scoped_lock _(m_map_mutex);

    if (is_master_client) {
        auto entity_list = sdk::EntityList::get();

        if (entity_list == nullptr) {
            return;
        }

        for (auto i = 0; i < entity_list->size(); ++i) {
            auto container = entity_list->get(i);

            if (container == nullptr) {
                continue;
            }

            auto behavior = container->behavior;

            if (behavior == nullptr) {
                continue;
            }

            // Send any existing valid entities
            // that are not currently networked to the server.
            if (!m_handle_map.contains(container->handle) && behavior->is_networkable()) {
                spdlog::info("Sending existing entity {}", container->name);

                sdk::EntitySpawnParams spawn_params{};
                sdk::EntitySpawnParams::PositionalData positional_data{};
                spawn_params.name = container->name;
                spawn_params.model = behavior->model_index();
                spawn_params.model2 = behavior->model_index();

                positional_data.position = Vector4f{behavior->position(), 1.0f};
                spawn_params.matrix = &positional_data;

                on_entity_created(container, &spawn_params);
            }
        }
    }
} catch (...) {
}

std::shared_ptr<NetworkEntity> EntitySync::add_entity(sdk::Entity* entity, uint32_t guid) {
    spdlog::info("Adding entity {:x} with guid {}", (uintptr_t)entity, guid);

    scoped_lock _(m_map_mutex);

    // This allows someone taking over as master client
    // to not screw up any previously spawned entities.
    if (guid > m_max_guid) {
        m_max_guid = guid;
    }

    auto& network_entity = m_network_entities[guid];

    if (network_entity == nullptr || network_entity->get_entity() != entity) {
        network_entity.reset();
        network_entity = std::make_shared<NetworkEntity>(entity, guid);
    }

    m_handle_map[entity->handle] = guid;
    network_entity->set_guid(guid);
    network_entity->set_entity(entity);

    nier::EntityData first_data(
        entity->behavior->facing(),
        //entity->behavior->as<sdk::Pl0000>()->character_controller().facing,
        0.0f,
        entity->behavior->as<sdk::BehaviorAppBase>()->health(),
        *(nier::Vector3f*)&entity->behavior->position()
    );

    network_entity->set_entity_data(first_data);

    return network_entity;
}

void EntitySync::remove_entity(uint32_t identifier) {
    scoped_lock _(m_map_mutex);

    if (m_network_entities.find(identifier) != m_network_entities.end()) {
        spdlog::info("Removing entity from EntitySync");
        m_network_entities.erase(identifier);
    }
}

void EntitySync::think() {
    scoped_lock _(m_map_mutex);

    for (auto& it : m_network_entities) {
        auto networked_entity = it.second;
        auto ent = networked_entity->get_entity();

        if (ent == nullptr) {
            continue;
        }

        auto& packet = networked_entity->get_entity_data();
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
            auto entity_list = sdk::EntityList::get();

            if (entity_list == nullptr) {
                return;
            }

            for (auto i = 0; i < entity_list->size(); ++i) {
                auto container = entity_list->get(i);

                if (container == nullptr) {
                    continue;
                }

                auto behavior = container->behavior;

                if (behavior == nullptr) {
                    continue;
                }

                // Delete any entities that are not supposed to be networked.
                if (!m_handle_map.contains(container->handle) && behavior->is_networkable()) {
                    spdlog::info("Deleting entity {:x} {}", (uintptr_t)container, container->name);
                    behavior->terminate();
                }
            }
        }
    } catch(...) {

    }
}

void EntitySync::process_entity_data(uint32_t guid, const nier::EntityData* data) {
    //spdlog::info("Processing {} entity data", guid);

    scoped_lock _(m_map_mutex);
    if (auto it = m_network_entities.find(guid); it != m_network_entities.end()) {
        auto ent = it->second;

        const auto cont = ent->get_entity();

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

        ent->set_entity_data(*data);
    }
}
