#pragma once
namespace regenny {
struct UnkEntityData;
}
namespace regenny {
struct Entity;
}
namespace regenny {
#pragma pack(push, 1)
struct EntityContainer {
    regenny::UnkEntityData* unknown; // 0x0
    // Metadata: utf8*
    char name[32]; // 0x8
    uint64_t flags; // 0x28
    uint32_t handle; // 0x30
    char pad_34[0x4];
    void* unknown2; // 0x38
    void* raw_file_data; // 0x40
    regenny::Entity* entity; // 0x48
    char pad_50[0xfb0];
}; // Size: 0x1000
#pragma pack(pop)
}
