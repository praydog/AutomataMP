#pragma once
#include "Parts.hpp"
#include "Vector4f.hpp"
namespace regenny {
#pragma pack(push, 1)
struct Model : public Parts {
    char pad_8[0x48];
    regenny::Vector4f position; // 0x50
    char pad_60[0x34];
    float facing; // 0x94
    char pad_98[0x4];
    float tick_count; // 0x9c
    char pad_a0[0x4f0];
}; // Size: 0x590
#pragma pack(pop)
}
