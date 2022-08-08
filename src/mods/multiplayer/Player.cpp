#include <sdk/EntityList.hpp>
#include <sdk/Entity.hpp>
#include "Player.hpp"

sdk::Pl0000* Player::getEntity() {
    if (!m_entityHandle) {
        return nullptr;
    }

    auto ent = sdk::EntityList::get()->getByHandle(m_entityHandle);

    if (!ent) {
        return nullptr;
    }

    return ent->behavior->as<sdk::Pl0000>();
}
