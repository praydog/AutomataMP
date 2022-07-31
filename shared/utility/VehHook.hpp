#pragma once

#include <functional>
#include <vector>
#include <cstdint>
#include <mutex>

#include <Windows.h>

#include "PointerHook.hpp"
#include "Address.hpp"

class VehHook {
public:
    struct RuntimeInfo;

    typedef std::function<void(const RuntimeInfo&)> CallbackFn;

    struct HookInfo {
        Address target;
        Address next; // The next instruction after the target instruction.
        Address destination;

        uint8_t originalByte;
        uint8_t nextOriginalByte;

        CallbackFn cb;
    };

    struct RuntimeInfo {
        PCONTEXT context;
        HookInfo& info;
    };

    VehHook();
    virtual ~VehHook();

    void create();
    void remove();

    bool hook(Address target, CallbackFn cb);
    bool hook(Address target, Address destination, CallbackFn cb);

    auto getHookResetFunction(const RuntimeInfo& hook) {
        auto target = hook.info.target;
        return [=]() {
            ProtectionOverride prot{target, 1, PAGE_EXECUTE_READWRITE};
            *target.as<uint8_t*>() = 0xCC;
        };
    }

private:
    static inline std::recursive_mutex s_mutex{};
    std::vector<HookInfo> m_hooks;
    PVOID m_handler;

    static LONG CALLBACK handler(PEXCEPTION_POINTERS info);
};