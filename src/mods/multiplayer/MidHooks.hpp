#pragma once

#include <unordered_set>
#include <functional>
#include <mutex>
#include <memory>

#include <asmjit/asmjit.h>

#include <safetyhook/MidHook.hpp>
#include <safetyhook/InlineHook.hpp>
#include <utility/VehHook.hpp>

namespace sdk {
class Entity;
class Behavior;
struct EntitySpawnParams;
}

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

    void on_processed_buttons(safetyhook::Context& info);
    void on_pre_entity_spawn(safetyhook::Context& info);
    void on_post_entity_spawn(safetyhook::Context& info);
    sdk::Entity* on_entity_spawn(void* rcx, void* rdx);
    void on_entity_terminate(safetyhook::Context& info);
    void on_update(safetyhook::Context& info);

    void add_overriden_entity(sdk::Behavior* ent);

    typedef void (MidHooks::*MemberMidCallbackFn)(safetyhook::Context&);
    typedef void (MidHooks::*MemberInlineCallbackFn)(HookAndParams&);

    static inline bool s_ignore_spawn{false};

private:
    void add_hook(uintptr_t address, MemberMidCallbackFn cb);
    void add_hook(uintptr_t address, safetyhook::MidHookFn cb);
    void add_hook(uintptr_t address, MemberInlineCallbackFn cb); // NOT thread safe yet!!!!

    safetyhook::InlineHook* add_inline_hook(uintptr_t address, void* cb);

private:
    std::recursive_mutex m_hook_mutex{};

    asmjit::JitRuntime m_jit{};
    std::vector<std::unique_ptr<safetyhook::MidHook>> m_mid_hooks;
    std::vector<std::unique_ptr<HookAndParams>> m_inline_hooks_with_params;
    std::vector<std::unique_ptr<safetyhook::InlineHook>> m_inline_hooks;

    static sdk::Entity* entity_spawn_hook(void* rcx, void* rdx);
    safetyhook::InlineHook* m_entity_spawn_hook{};

    std::unordered_set<sdk::Behavior*> m_overriden_entities;

    std::recursive_mutex m_spawn_mutex;
    std::unordered_map<uint32_t, sdk::EntitySpawnParams*> m_thread_id_to_spawn_params;
};