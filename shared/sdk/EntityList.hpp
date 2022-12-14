#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <tuple>
#include <sdk/Math.hpp>

namespace sdk {
class Entity;

struct EntityDescriptor {
    uint32_t handle;
    Entity* ent;
};

struct EntitySpawnParams {
    struct PositionalData {
        Vector4f forward { 1.0f, 0.0f, 0.0f, 0.0f }; // 0 - 10
        Vector4f up      { 0.0f, 1.0f, 0.0f, 0.0f }; // 10 - 20
        Vector4f right   { 0.0f, 0.0f, 1.0f, 0.0f }; // 20 - 30
        Vector4f w       { 0.0f, 0.0f, 0.0f, 1.0f }; // 30 - 40
        Vector4f position{ 0.0f, 0.0f, 0.0f, 1.0f }; // 40 - 50
        Vector4f unknown { 0.0f, 0.0f, 0.0f, 0.0f }; // 50 - 60
        Vector4f unknown2{ 1.0f, 1.0f, 1.0f, 0.0f }; // 60 - 70
        uint32_t unk{ 0 }; // 70-74
        uint32_t unk2{ 0 }; // 70 - 78
        uint32_t unk3{ 0 }; // 78 - 7c
        uint32_t unk4{ 0 }; // 7c - 80
        uint32_t unk5{ 0 }; // 80 - 84
        uint32_t unk6{ 0 }; // 84 - 88
        uint32_t unk7{ 1 }; // 88 - 8c
        uint32_t unk8{ 0 }; // 8c - 90
    };

    const char* name; // 0-8
    uint32_t model; // 8 -c
    uint32_t model2; // c - 10
    PositionalData* matrix{ nullptr }; // 10 - 18
    uint64_t unk1{ 0 }; // 18 - 20
    uint32_t unk2{ 1 }; // 20 - 24 // copied from positionaldata + 0x88?
    uint64_t unk3{ 0 }; // 28 - 30
    Vector4f unk4{ 0.0f, 0.0f, 0.0f, 0.0f }; // 30 - 38
    Vector4f unk5{ 0.0f, 0.0f, 0.0f, 0.0f }; // 38 - 40
    uint64_t unk6{ 0 }; // 40 -48
    char crap[0x28]{ 0 };
};

class EntityList {
public:
    static EntityList* get();

public:
    Entity* get(uint32_t i);
    Entity* get_by_name(const std::string& name);
    Entity* get_by_handle(uint32_t handle);
    Entity* get_possessed_entity();

    using SpawnFunction = Entity* (*)(void*, const EntitySpawnParams&);
    using SpawnThis = void*;
    static std::tuple<SpawnFunction, SpawnThis> get_spawn_entity_fn();
    static void* get_post_spawn_entity_fn();

    Entity* spawn_entity(const EntitySpawnParams& params);
    Entity* spawn_entity(const std::string& name, uint32_t model, const Vector3f& position);

    std::vector<Entity*> get_all_by_name(const std::string& name);

public:
    size_t size();

    EntityDescriptor* begin() {
        return &m_entities[0];
    }

    EntityDescriptor* end() {
        return begin() + size();
    }

private:
    EntityDescriptor m_entities[1];
};
}