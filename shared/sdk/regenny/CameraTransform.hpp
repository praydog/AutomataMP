#pragma once
#include "Matrix4x4f.hpp"
#include "Vector3f.hpp"
namespace regenny {
#pragma pack(push, 1)
struct CameraTransform {
    char pad_8[0x8];
    regenny::Vector3f pos; // 0x10
    regenny::Vector3f pos2; // 0x1c
    regenny::Vector3f pos3; // 0x28
    char pad_34[0x2c];
    virtual regenny::Matrix4x4f destructor() = 0;
}; // Size: 0x60
#pragma pack(pop)
}
