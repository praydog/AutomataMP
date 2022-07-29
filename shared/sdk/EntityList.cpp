#include <Windows.h>

#include <spdlog/spdlog.h>

#include <sdk/Entity.hpp>
#include <sdk/EntityList.hpp>

using namespace std;

EntityList* EntityList::get() {
    static EntityList** entList = nullptr;
    
    if (!entList)
    {
        entList = Address(GetModuleHandle(0)).get(0x160DF98).as<EntityList**>();
        spdlog::info("EntityList: {:x}", (uintptr_t)entList);
    }

    return *entList;
}

EntityContainer* EntityList::get(uint32_t i)
{
    return m_entities[i].ent;
}

EntityContainer* EntityList::getByName(const std::string& name) {
    for (auto& i : *this) {
        if (!i.ent)
            continue;

        if (name == i.ent->name)
            return i.ent;
    }

    return nullptr;
}

EntityContainer* EntityList::getByHandle(uint32_t handle) {
    auto index = (uint16_t)(handle >> 8);
    return m_entities[index].ent;
}

EntityContainer* EntityList::getPossessedEntity() {
    static uint32_t* currentHandle = Address(0x14158A6EC).as<uint32_t*>();
    
    if (!*currentHandle) {
        return nullptr;
    }

    return getByHandle(*currentHandle);
}

EntityContainer* EntityList::spawnEntity(const EntitySpawnParams& params) {
    static EntityContainer* (*spawn)(void*, const EntitySpawnParams&) = (decltype(spawn))0x1404F9AA0;
    return spawn((void*)0x14160DFE0, params);
}

EntityContainer* EntityList::spawnEntity(const std::string& name, uint32_t model, const Vector3f& position) {
    EntitySpawnParams params;
    EntitySpawnParams::PositionalData matrix;
    matrix.position = Vector4f{position, 1.0f};
    params.name = (char*)name.c_str();
    params.matrix = &matrix;
    params.model = model;
    params.model2 = model;
    matrix.unknown = *Address(getByName("Player")->entity).get(0x90).as<Vector4f*>();

    auto ret = spawnEntity(params);
    return ret;
}

std::vector<EntityContainer*> EntityList::getAllByName(const std::string& name) {
    std::vector<EntityContainer*> vec;

    for (auto& i : *this) {
        if (!i.ent)
            continue;

        if (name == i.ent->name) {
            vec.push_back(i.ent);
        }
    }

    return vec;
}

size_t EntityList::size() {
    static uint32_t* s = nullptr;

    if (!s) {
        s = Address(GetModuleHandle(0)).get(0x160DF88).as<uint32_t*>();
        spdlog::info("size: {:x}", (uintptr_t)s);
    }

    return *s;
}
