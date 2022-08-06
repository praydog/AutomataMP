#pragma once
#include "Vector4f.hpp"
namespace regenny {
#pragma pack(push, 1)
struct Matrix4x4f {
    regenny::Vector4f x; // 0x0
    regenny::Vector4f y; // 0x10
    regenny::Vector4f z; // 0x20
    regenny::Vector4f w; // 0x30
}; // Size: 0x40
#pragma pack(pop)
}
