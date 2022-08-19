#pragma once
namespace regenny {
#pragma pack(push, 1)
struct RTL_CRITICAL_SECTION {
    void* DebugInfo; // 0x0
    int32_t LockCount; // 0x8
    int32_t RecursionCount; // 0xc
    void* OwningThread; // 0x10
    void* LockSemaphore; // 0x18
    uint32_t SpinCount; // 0x20
}; // Size: 0x24
#pragma pack(pop)
}
