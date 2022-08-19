#include <sdk/EntityList.hpp>
#include <sdk/Entity.hpp>
#include "Player.hpp"

sdk::Pl0000* Player::get_entity() {
    if (!m_entity_handle) {
        return nullptr;
    }

    auto ent = sdk::EntityList::get()->getByHandle(m_entity_handle);

    if (!ent) {
        return nullptr;
    }

    return ent->behavior->as<sdk::Pl0000>();
}
