#pragma once
#include "BehaviorExtension.hpp"
namespace regenny {
#pragma pack(push, 1)
struct ExCatch : public BehaviorExtension {
    char pad_10[0x70];
}; // Size: 0x80
#pragma pack(pop)
}
