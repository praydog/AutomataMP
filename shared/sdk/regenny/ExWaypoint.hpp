#pragma once
#include "BehaviorExtension.hpp"
namespace regenny {
#pragma pack(push, 1)
struct ExWaypoint : public BehaviorExtension {
    char pad_10[0xa0];
}; // Size: 0xb0
#pragma pack(pop)
}
