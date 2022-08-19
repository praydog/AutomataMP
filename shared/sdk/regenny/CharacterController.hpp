#pragma once
#include "RTL_CRITICAL_SECTION.hpp"
namespace regenny {
#pragma pack(push, 1)
struct CharacterController {
    char pad_0[0x88];
    regenny::RTL_CRITICAL_SECTION critical_section; // 0x88
    char pad_ac[0x68c];
    uint32_t held_flags; // 0x738
    uint32_t flags; // 0x73c
    char pad_740[0x30];
    uint32_t buttons[8]; // 0x770
    char pad_790[0x4];
    float speed; // 0x794
    char pad_798[0x4];
    float facing; // 0x79c
    char pad_7a0[0xb0];
}; // Size: 0x850
#pragma pack(pop)
}
