#pragma once
#include "Behavior.hpp"
namespace regenny {
#pragma pack(push, 1)
struct BehaviorAppBase : public Behavior {
    char pad_830[0x28];
    uint32_t health; // 0x858
    char pad_85c[0x3f4];
}; // Size: 0xc50
#pragma pack(pop)
}
