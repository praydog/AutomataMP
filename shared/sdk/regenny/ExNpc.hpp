#pragma once
#include "BehaviorExtension.hpp"
namespace regenny {
#pragma pack(push, 1)
struct ExNpc : public BehaviorExtension {
    char pad_10[0x28];
}; // Size: 0x38
#pragma pack(pop)
}
