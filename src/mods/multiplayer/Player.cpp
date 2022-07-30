#include <sdk/EntityList.hpp>
#include <sdk/Entity.hpp>
#include "Player.hpp"

Entity* Player::getEntity() {
    if (!m_entityHandle) {
        return nullptr;
    }

    auto ent = EntityList::get()->getByHandle(m_entityHandle);

    if (!ent) {
        return nullptr;
    }

    return ent->entity;
}
