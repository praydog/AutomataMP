#pragma once
#include "BehaviorExtension.hpp"
namespace regenny {
#pragma pack(push, 1)
struct ExActionState : public BehaviorExtension {
    char pad_10[0x18];
}; // Size: 0x28
#pragma pack(pop)
}
