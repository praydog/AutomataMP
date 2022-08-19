#pragma once
namespace regenny {
#pragma pack(push, 1)
struct ExBaseInfo {
    char pad_0[0x130];
    void* wp_anim_name_array; // 0x130
    char pad_138[0xc0];
}; // Size: 0x1f8
#pragma pack(pop)
}
