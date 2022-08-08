#pragma once
#include "CameraApp.hpp"
namespace regenny {
#pragma pack(push, 1)
struct CameraGame : public CameraApp {
    char pad_370[0x88b0];
}; // Size: 0x8c20
#pragma pack(pop)
}
