#pragma once
#include "BehaviorExtension.hpp"
namespace regenny {
#pragma pack(push, 1)
struct ExLockOn : public BehaviorExtension {
    char pad_10[0xe0];
}; // Size: 0xf0
#pragma pack(pop)
}
