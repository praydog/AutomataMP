#pragma once
#include "BehaviorAppBase.hpp"
#include "CharacterController.hpp"
#include "ERunSpeedType.hpp"
#include "ExAtkCombo.hpp"
#include "ExBaseInfo.hpp"
#include "ExCatch.hpp"
#include "ExCollision.hpp"
#include "ExExpInfo.hpp"
#include "ExHackingInfo.hpp"
#include "ExLayoutParent.hpp"
#include "ExLockOn.hpp"
#include "cObjHit.hpp"
namespace regenny {
#pragma pack(push, 1)
struct Pl0000 : public BehaviorAppBase {
    char pad_c50[0x50];
    regenny::CharacterController controller; // 0xca0
    regenny::cObjHit obj_hit; // 0x14f0
    char pad_1780[0x10];
    regenny::ExCollision ex_collision_1; // 0x1790
    regenny::ExCollision ex_collision_2; // 0x18c0
    char pad_19f0[0x490];
    regenny::ExBaseInfo ex_base_info; // 0x1e80
    regenny::ExExpInfo ex_exp_info; // 0x2078
    regenny::ExCatch ex_catch; // 0x2b70
    char pad_2bf0[0xd440];
    regenny::ExLockOn ex_lock_on; // 0x10030
    char pad_10120[0x170];
    void* obj_array_1; // 0x10290
    char pad_10298[0x540];
    uint32_t weapon_index; // 0x107d8
    char pad_107dc[0x44];
    uint32_t pod_index; // 0x10820
    char pad_10824[0x1ebc];
    regenny::ExLayoutParent ex_layout_parent; // 0x126e0
    char pad_127f0[0x60];
    regenny::ExHackingInfo ex_hacking_info; // 0x12850
    char pad_128d0[0x308];
    void* obj_array_2; // 0x12bd8
    char pad_12be0[0x120];
    void* obj_array_3; // 0x12d00
    char pad_12d08[0xc8];
    void* flight_missile_array_1; // 0x12dd0
    char pad_12dd8[0x48];
    void* flight_missile_array_2; // 0x12e20
    char pad_12e28[0xa8];
    void* obj_array_4; // 0x12ed0
    char pad_12ed8[0x218];
    regenny::ExAtkCombo ex_atk_combo; // 0x130f0
    char pad_13100[0x2dd8];
    void* entity_handle_array_1; // 0x15ed8
    char pad_15ee0[0x1b0];
    void* obj_array_5; // 0x16090
    char pad_16098[0x3e4];
    uint32_t buddy_handle; // 0x1647c
    char pad_16480[0x8];
    regenny::ERunSpeedType run_speed_type; // 0x16488
    char pad_1648c[0x104];
    void* entity_handle_array_2; // 0x16590
    char pad_16598[0x70];
    void* passive_skill_array; // 0x16608
    char pad_16610[0x6d4];
    bool shorts_torn; // 0x16ce4
    char pad_16ce5[0x7];
    bool wearing_mask; // 0x16cec
    char pad_16ced[0x127];
    uint32_t controlled_entity_maybe; // 0x16e14
    char pad_16e18[0x698];
    bool flashlight; // 0x174b0
    char pad_174b1[0x46f];
}; // Size: 0x17920
#pragma pack(pop)
}
