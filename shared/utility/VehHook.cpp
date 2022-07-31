#include <algorithm>
#include <mutex>

#include <hde64.h>

#include "VehHook.hpp"

using namespace std;

vector<VehHook*> g_vehHooks;
mutex g_vehHooksMutex;
PVOID g_vehHandler = nullptr;

bool write(Address address, const void* data, size_t size) {
    ProtectionOverride prot{ address, size, PAGE_EXECUTE_READWRITE };
    memcpy((void*)address, data, size);

    return true;
}

bool read(Address address, void* data, size_t size) {
    if (address.as<void*>() == nullptr) {
        return false;
    }
    memcpy(data, address.as<void*>(), size);
    return true;
}

static int hde(Address address) {
    hde64s hs;
    return hde64_disasm(address, &hs);
}

VehHook::VehHook()
    : m_hooks()
{
    lock_guard<mutex> _(g_vehHooksMutex);

    g_vehHooks.push_back(this);
    create();
}

VehHook::~VehHook() {
    lock_guard<mutex> _(g_vehHooksMutex);

    remove();
    g_vehHooks.erase(std::remove(g_vehHooks.begin(), g_vehHooks.end(), this), g_vehHooks.end());
}

void VehHook::create() {
    if (!g_vehHandler) {
        g_vehHandler = AddVectoredExceptionHandler(1, &VehHook::handler);
    }
}

void VehHook::remove() {
    // Remove the int3's.
    for (auto& hook : m_hooks) {
        write(hook.target, &hook.originalByte, 1);
        write(hook.next, &hook.nextOriginalByte, 1);
    }

    m_hooks.clear();
}

bool VehHook::hook(Address target, CallbackFn cb) {
    return hook(target, nullptr, cb);
}

bool VehHook::hook(Address target, Address destination, CallbackFn cb) {
    HookInfo ctx;

    ctx.target = target;
    ctx.next = target + hde(target);
    ctx.destination = destination;
    ctx.cb = cb;

    if (!read(ctx.target, &ctx.originalByte, 1)) {
        return false;
    }
    
    if (!read(ctx.next, &ctx.nextOriginalByte, 1)) {
        return false;
    }

    uint8_t int3 = 0xCC;

    if (!write(ctx.target, &int3, 1)) {
        return false;
    }

    m_hooks.push_back(ctx);

    return true;
}

LONG VehHook::handler(PEXCEPTION_POINTERS info) {
    std::scoped_lock _{VehHook::s_mutex};

    auto record = info->ExceptionRecord;
    auto code = record->ExceptionCode;


    if (code == EXCEPTION_BREAKPOINT) {
        auto context = info->ContextRecord;
        auto ip = context->Rip;

        for (auto& hook : g_vehHooks) {
            for (auto& ctx : hook->m_hooks) {
                if (ip != ctx.target.as<decltype(ip)>() && ip != ctx.next.as<decltype(ip)>()) {
                    continue;
                }

                if (ip == ctx.target) {
                    // Remove the int3.
                    if (!write(ctx.target, &ctx.originalByte, 1)) {
                        continue;
                    }

                    // Call the callback.
                    if (ctx.cb) {
                        ctx.cb(RuntimeInfo{ context, ctx });

                        // Write an int3 to the next instruction so that it can 
                        // restore the targets int3.
                        uint8_t int3 = 0xCC;

                        write(ctx.next, &int3, 1);
                    }

                    // Call the hook.
                    if (ctx.destination) {
                        context->Rip = ctx.destination;
                    }
                }
                else {
                    // Remove the int3.
                    write(ctx.next, &ctx.nextOriginalByte, 1);

                    // Restore the int3 to the target address.
                    uint8_t int3 = 0xCC;

                    write(ctx.target, &int3, 1);
                }
                
                return EXCEPTION_CONTINUE_EXECUTION;
            }
        }
    }

    return EXCEPTION_CONTINUE_SEARCH;
}
