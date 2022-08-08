#pragma once
#include "Camera.hpp"
namespace regenny {
struct CameraData;
}
namespace regenny {
#pragma pack(push, 1)
struct CameraApp : public Camera {
    char pad_70[0xc8];
    float fov; // 0x138
    char pad_13c[0x234];
    virtual ~CameraApp() = default;
    virtual void virtual_function_1() = 0;
    virtual regenny::CameraData* cameraAppLastVirtual() = 0;
    virtual void virtual_function_3() = 0;
}; // Size: 0x370
#pragma pack(pop)
}
