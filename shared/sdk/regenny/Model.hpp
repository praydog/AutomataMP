#pragma once
#include "Matrix4x4f.hpp"
#include "Parts.hpp"
#include "Vector4f.hpp"
#include "cModelExtendWork.hpp"
namespace regenny {
#pragma pack(push, 1)
struct Model : public Parts {
    char pad_8[0x48];
    regenny::Vector4f position; // 0x50
    char pad_60[0x34];
    float facing; // 0x94
    char pad_98[0x4];
    float tick_count; // 0x9c
    char pad_a0[0x20];
    regenny::Matrix4x4f model_mat4_1; // 0xc0
    regenny::Matrix4x4f model_mat4_2; // 0x100
    regenny::cModelExtendWork model_extend; // 0x140
    char pad_390[0x1e8];
    float _578; // 0x578
    float _57C; // 0x57c
    char pad_580[0x10];
}; // Size: 0x590
#pragma pack(pop)
}
