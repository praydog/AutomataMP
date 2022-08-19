#pragma once
#include "BehaviorExtension.hpp"
namespace regenny {
#pragma pack(push, 1)
struct ExLayoutParent : public BehaviorExtension {
    char pad_10[0x100];
}; // Size: 0x110
#pragma pack(pop)
}
