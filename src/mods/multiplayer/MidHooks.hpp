#pragma once

#include <unordered_set>
#include <functional>
#include <mutex>
#include <memory>

#include <asmjit/asmjit.h>

#include <safetyhook/MidHook.hpp>
#include <safetyhook/InlineHook.hpp>
#include <utility/VehHook.hpp>

class Entity;
struct EntitySpawnParams;
struct EntityContainer;

struct HookAndParams {
    void* rcx;
    void* rdx;
    void* r8;
    void* r9;
    std::unique_ptr<safetyhook::InlineHook> hook;
};

class MidHooks {
public:
    MidHooks();

    void onLightAttack(const VehHook::RuntimeInfo& info);
    void onHeavyAttack(const VehHook::RuntimeInfo& info);
    void onProcessedButtons(safetyhook::Context& info);
    void onPreEntitySpawn(safetyhook::Context& info);
    void onPostEntitySpawn(safetyhook::Context& info);
    EntityContainer* onEntitySpawn(HookAndParams& hookAndParams);
    void onEntityTerminate(safetyhook::Context& info);
    void onUpdate(safetyhook::Context& info);

    void addOverridenEntity(Entity* ent);

    typedef void (MidHooks::*MemberMidCallbackFn)(safetyhook::Context&);
    typedef void (MidHooks::*MemberInlineCallbackFn)(HookAndParams&);

private:
    void addHook(uintptr_t address, MemberMidCallbackFn cb);
    void addHook(uintptr_t address, safetyhook::MidHookFn cb);
    void addHook(uintptr_t address, MemberInlineCallbackFn cb);

private:
    std::recursive_mutex m_hookMutex{};
    
    asmjit::JitRuntime m_jit{};
    std::vector<std::unique_ptr<safetyhook::MidHook>> m_midHooks;
    std::vector<std::unique_ptr<HookAndParams>> m_inlineHooks;

    std::unordered_set<Entity*> m_overridenEntities;

    std::recursive_mutex m_spawnMutex;
    std::unordered_map<uint32_t, EntitySpawnParams*> m_threadIdToSpawnParams;
};