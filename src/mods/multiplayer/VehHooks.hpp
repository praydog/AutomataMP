#pragma once

#include <unordered_set>
#include <functional>
#include <mutex>

#include <utility/VehHook.hpp>

class Entity;
struct EntitySpawnParams;

class VehHooks {
public:
    VehHooks();

    void onLightAttack(const VehHook::RuntimeInfo& info);
    void onHeavyAttack(const VehHook::RuntimeInfo& info);
    void onProcessedButtons(const VehHook::RuntimeInfo& info);
    void onPreEntitySpawn(const VehHook::RuntimeInfo& info);
    void onPostEntitySpawn(const VehHook::RuntimeInfo& info);
    void onEntityTerminate(const VehHook::RuntimeInfo& info);
    void onUpdate(const VehHook::RuntimeInfo& info);

    void addOverridenEntity(Entity* ent);

    auto& getHook() {
        return m_hook;
    }

    typedef void (VehHooks::*MemberCallbackFn)(const VehHook::RuntimeInfo&);

private:
    void addHook(uintptr_t address, MemberCallbackFn cb);

private:
    VehHook m_hook;
    std::unordered_set<Entity*> m_overridenEntities;

    std::mutex m_spawnMutex;
    std::unordered_map<uint32_t, EntitySpawnParams*> m_threadIdToSpawnParams;
};