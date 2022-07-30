#pragma once

#include <cstdint>
#include <vector>

#include <sdk/Math.hpp>
#include <enet/enet.h>

#include <sdk/Entity.hpp>
#include <sdk/EntityList.hpp>

struct Packet {
    uint32_t id{ 0 };

    enet_uint8* data() {
        return (enet_uint8*)this;
    }
};

template <uint32_t idT = 0>
struct PacketT : public Packet {
    PacketT() { id = idT; }
};

enum {
    ID_SERVER_START = 1,
    ID_SPAWN_ENTITY = ID_SERVER_START,
    ID_ENTITY_DATA,
    ID_SERVER_END,
    ID_SHARED_START = 100,
    ID_PLAYER_DATA = ID_SHARED_START,
    ID_ANIMATION_START,
    ID_CHANGE_PLAYER,
    ID_BUTTONS,
    ID_SHARED_END,
};

namespace nier_client_and_server {
    struct PlayerData : public PacketT<ID_PLAYER_DATA> {
        bool flashlight;
        float speed;
        float facing;
        float facing2;
        uint32_t weaponIndex;
        uint32_t podIndex;
        uint32_t heldButtonFlags;
        Vector3f position;
    };

    struct AnimationStart : public PacketT<ID_ANIMATION_START> {
        uint32_t anim;
        uint32_t variant;
        uint32_t a3;
        uint32_t a4;
    };

    struct ChangePlayer : public PacketT<ID_CHANGE_PLAYER> { };

    struct Buttons : public PacketT<ID_BUTTONS> {
        uint32_t buttons[Entity::CharacterController::INDEX_MAX];
    };
}

namespace nier_client {

}

namespace nier_server {
    template<uint32_t idT>
    struct EntityPacket : public PacketT<idT> {
        uint32_t guid;
    };

    struct EntitySpawn : public EntityPacket<ID_SPAWN_ENTITY> {
        char name[0x20];
        uint32_t model;
        uint32_t model2;
        EntitySpawnParams::PositionalData matrix;
    };

    struct EntityData : public EntityPacket<ID_ENTITY_DATA> {
        float facing;
        float facing2;
        uint32_t health;
        Vector3f position;
    };
}
