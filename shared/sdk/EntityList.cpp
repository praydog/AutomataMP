#include <Windows.h>

#include <spdlog/spdlog.h>

#include <utility/Scan.hpp>
#include <utility/Module.hpp>

#include <sdk/Entity.hpp>
#include <sdk/EntityList.hpp>

using namespace std;

EntityList** get_entity_list_base() {
    // old version ~2017
    /*static EntityList** entList = nullptr;
    
    if (!entList)
    {
        entList = Address(GetModuleHandle(0)).get(0x160DF98).as<EntityList**>();
        spdlog::info("EntityList: {:x}", (uintptr_t)entList);
    }*/

    static EntityList** entity_list = []() -> EntityList** {
        spdlog::info("[EntityList] Finding EntityList...");

        EntityList** result{nullptr};

        const auto game = utility::get_executable();
        const auto ref = utility::scan(game, "48 c1 e1 04 48 03 ? ? ? ? ? 8b 01 33 c2 a9 00 ff ff ff 75 0f 48 8b 41 08");

        if (!ref) {
            spdlog::error("[EntityList] Failed to find EntityList.");
            return nullptr;
        }

        spdlog::info("[EntityList] Ref: {:x}", (uintptr_t)*ref);

        result = (EntityList**)utility::calculate_absolute(*ref + 7);

        spdlog::info("[EntityList] EntityList: {:x}", (uintptr_t)result);

        return result;
    }();

    return entity_list;
}

EntityList* EntityList::get() {
    return *get_entity_list_base();
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
        // old version ~2017
        //s = Address(GetModuleHandle(0)).get(0x160DF88).as<uint32_t*>();
        s = (uint32_t*)((uintptr_t)get_entity_list_base() - 0x10);
        spdlog::info("size: {:x}", (uintptr_t)s);
    }

    return *s;
}
