#pragma once
#include "BehaviorExtension.hpp"
namespace regenny {
#pragma pack(push, 1)
struct ExCollision : public BehaviorExtension {
    char pad_10[0x120];
}; // Size: 0x130
#pragma pack(pop)
}
