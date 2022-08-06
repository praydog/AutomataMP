#pragma once
#include "Vector3f.hpp"
namespace regenny {
struct CameraData;
}
namespace regenny {
#pragma pack(push, 1)
struct CameraGame {
    void* vftable; // 0x0
    char pad_8[0x8];
    regenny::Vector3f pos; // 0x10
    regenny::Vector3f pos2; // 0x1c
    regenny::Vector3f pos3; // 0x28
    char pad_34[0x2c];
    regenny::CameraData* data; // 0x60
    char pad_68[0xd0];
    float fov; // 0x138
    char pad_13c[0xec4];
}; // Size: 0x1000
#pragma pack(pop)
}
