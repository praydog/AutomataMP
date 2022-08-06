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
    //static uint32_t* currentHandle = Address(0x14158A6EC).as<uint32_t*>(); // old 2017
    
    /*
    7FF710F48FBA - 0x57 Taura_TestRoom
    7FF710FA104F + 0x54 se_hacking_in
    7FF710F9A864 + 0x52 PauseMap:PL
    7FF710F4EF35 + 0x49 item_recSt_Drk
    7FF710F36207 + 0x3C EmShootingItem
    7FF710F94F43 + 0x28 00_80_A_RobotM_Pro_1stArea
    7FF710FA1830 + 0x26 M1080S0080b_SYSTEMHACK_1
    7FF710F60402 + 0xE core_keyitem_get
    7FF710F923BE + 0x9 core_AccelTime_Out
    */
    static uint32_t* currentHandle = []() -> uint32_t* {
        spdlog::info("[EntityList] Finding currentHandle...");

        const auto str = utility::scan_string(utility::get_executable(), "core_AccelTime_Out");

        if (!str) {
            spdlog::error("[EntityList] Failed to find currentHandle.");
            return nullptr;
        }

        spdlog::info("[EntityList] str: {:x}", (uintptr_t)*str);

        const auto str_ref = utility::scan_reference(utility::get_executable(), *str);

        if (!str_ref) {
            spdlog::error("[EntityList] Failed to find currentHandle.");
            return nullptr;
        }

        spdlog::info("[EntityList] str_ref: {:x}", (uintptr_t)*str_ref);

        const auto ref = utility::scan_disasm(*str_ref + 4, 10, "48 8D 15");

        if (!ref) {
            spdlog::error("[EntityList] Failed to find currentHandle.");
            return nullptr;
        }

        spdlog::info("[EntityList] ref: {:x}", (uintptr_t)*ref);

        uint32_t* result = (uint32_t*)utility::calculate_absolute(*ref + 3);

        spdlog::info("[EntityList] currentHandle: {:x}", (uintptr_t)result);

        return result;
    }();
    
    if (!*currentHandle) {
        return nullptr;
    }

    return getByHandle(*currentHandle);
}

std::tuple<EntityList::SpawnFunction, EntityList::SpawnThis> EntityList::getSpawnEntityFn() {
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

    static auto out = []() -> std::tuple<EntityList::SpawnFunction, EntityList::SpawnThis> {
        spdlog::info("[EntityList] Finding spawn...");

        EntityList::SpawnFunction result{nullptr};
        EntityList::SpawnThis thisptr{nullptr};

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

        const auto ref2 = utility::scan_disasm(*ref + 4, 30, "48 8D 0D");

        if (!ref2) {
            spdlog::error("[EntityList] Failed to find spawn.");
            return {nullptr, nullptr};
        }

        spdlog::info("[EntityList] Ref2: {:x}", (uintptr_t)*ref2);

        thisptr = (EntityList::SpawnThis)utility::calculate_absolute(*ref2 + 3);
        result = (EntityList::SpawnFunction)utility::calculate_absolute(*ref2 + 8);

        spdlog::info("[EntityList] spawn: {:x}", (uintptr_t)result);
        spdlog::info("[EntityList] thisptr: {:x}", (uintptr_t)thisptr);

        return {result, thisptr};
    }();

    return out;
}

void* EntityList::getPostSpawnEntityFn() {
    static auto out = []() -> void* {
        spdlog::info("[EntityList] Finding postSpawn...");
        const auto [spawn, thisptr] = getSpawnEntityFn();

        spdlog::info("[EntityList] Scanning spawn function for ret instruction...");

        const auto ret = utility::scan_opcode((uintptr_t)spawn, 200, 0xC3);

        if (!ret) {
            spdlog::error("[EntityList] Failed to find postSpawn.");
            return nullptr;
        }

        spdlog::info("[EntityList] posSpawn: {:x}", (uintptr_t)*ret);

        return (void*)*ret;
    }();

    return out;
}

EntityContainer* EntityList::spawnEntity(const EntitySpawnParams& params) {
    auto [spawn, thisptr] = getSpawnEntityFn();
    return spawn(thisptr, params);
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
