#pragma once
namespace regenny {
#pragma pack(push, 1)
struct BehaviorExtension {
    void* vftable; // 0x0
    char pad_8[0x8];
}; // Size: 0x10
#pragma pack(pop)
}
