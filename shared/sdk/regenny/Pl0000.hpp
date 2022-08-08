#pragma once
#include "BehaviorAppBase.hpp"
#include "CharacterController.hpp"
#include "ERunSpeedType.hpp"
namespace regenny {
#pragma pack(push, 1)
struct Pl0000 : public BehaviorAppBase {
    char pad_c50[0x50];
    regenny::CharacterController controller; // 0xca0
    char pad_1ca0[0xeb38];
    uint32_t weapon_index; // 0x107d8
    char pad_107dc[0x44];
    uint32_t pod_index; // 0x10820
    char pad_10824[0x5c58];
    uint32_t buddy_handle; // 0x1647c
    char pad_16480[0x8];
    regenny::ERunSpeedType run_speed_type; // 0x16488
    char pad_1648c[0x858];
    bool shorts_torn; // 0x16ce4
    char pad_16ce5[0x7];
    bool wearing_mask; // 0x16cec
    char pad_16ced[0x7c3];
    bool flashlight; // 0x174b0
    char pad_174b1[0x46f];
}; // Size: 0x17920
#pragma pack(pop)
}
