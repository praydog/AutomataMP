#pragma once
#include "Matrix4x4f.hpp"
namespace regenny {
#pragma pack(push, 1)
struct CameraData {
    char pad_0[0x80];
    regenny::Matrix4x4f viewproj; // 0x80
    regenny::Matrix4x4f view; // 0xc0
    char pad_100[0xf00];
}; // Size: 0x1000
#pragma pack(pop)
}
