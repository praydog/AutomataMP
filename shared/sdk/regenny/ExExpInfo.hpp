#pragma once
#include "BehaviorExtension.hpp"
namespace regenny {
#pragma pack(push, 1)
struct ExExpInfo : public BehaviorExtension {
    char pad_10[0xae8];
}; // Size: 0xaf8
#pragma pack(pop)
}
