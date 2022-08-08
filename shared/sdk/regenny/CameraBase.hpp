#pragma once
#include "CameraTransform.hpp"
namespace regenny {
struct CameraData;
}
namespace regenny {
#pragma pack(push, 1)
struct CameraBase : public CameraTransform {
    regenny::CameraData* data; // 0x60
    char pad_68[0x8];
}; // Size: 0x70
#pragma pack(pop)
}
