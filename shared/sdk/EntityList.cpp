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
    // 2017 version
    /*static EntityContainer* (*spawn)(void*, const EntitySpawnParams&) = (decltype(spawn))0x1404F9AA0;
    return spawn((void*)0x14160DFE0, params);*/


    /*
        String refs
        7FF710F9614D + 0x56 ObjectParam.bxm
        7FF710F9616B + 0x38 MapInstance
    */

    /*
        String refs to easy func
        7FF710BE242C + 0xB PlFaceMask
        7FF710BE51B0 + 0xB Tower Elevator
        7FF710F8AE91 + 0xB FreeEnemy
        7FF710DBA636 + 0xB ba2100
        7FF710BE53C8 + 0xB Wp3000_Supplie
        7FF710BE554E + 0xB NPCWeapon
        7FF710D0312B + 0xB EmSHootingLaser
        7FF710F9F46F + 0xB Layout
        7FF710D019D5 + 0xA Em9010
        7FF710CFF99B + 0x4 Em0120
        7FF710CFFCDB + 0x4 Em1010
        7FF710CFF65B + 0x4 em0110
    */

    using spawn_t = EntityContainer* (*)(void*, const EntitySpawnParams&);
    static auto [spawn_fn, spawn_thisptr] = []() -> std::tuple<spawn_t, void*> {
        spdlog::info("[EntityList] Finding spawn...");

        spawn_t result{nullptr};
        void* thisptr{nullptr};

        const auto game = utility::get_executable();
        const auto str = utility::scan_string(game, "MapInstance");

        if (!str) {
            spdlog::error("[EntityList] Failed to find MapInstance.");
            return {nullptr, nullptr};
        }

        spdlog::info("[EntityList] MapInstance: {:x}", (uintptr_t)*str);

        const auto ref = utility::scan_reference(game, *str);
        
        if (!ref) {
            spdlog::error("[EntityList] Failed to find spawn.");
            return {nullptr, nullptr};
        }

        spdlog::info("[EntityList] Ref: {:x}", (uintptr_t)*ref);

        const auto ref2 = utility::scan_disasm(*ref, 30, "48 8D 0D");

        if (!ref2) {
            spdlog::error("[EntityList] Failed to find spawn.");
            return {nullptr, nullptr};
        }

        spdlog::info("[EntityList] Ref2: {:x}", (uintptr_t)*ref2);

        thisptr = (void*)utility::calculate_absolute(*ref2 + 3);
        result = (spawn_t)utility::calculate_absolute(*ref2 + 8);

        spdlog::info("[EntityList] spawn: {:x}", (uintptr_t)result);
        spdlog::info("[EntityList] thisptr: {:x}", (uintptr_t)thisptr);

        return {result, thisptr};
    }();

    return spawn_fn(spawn_thisptr, params);
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
