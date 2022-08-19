#pragma once
#include "Behavior.hpp"
#include "ExActionState.hpp"
#include "ExNpc.hpp"
#include "ExWaypoint.hpp"
namespace regenny {
struct Vector4f;
}
namespace regenny {
#pragma pack(push, 1)
struct BehaviorAppBase : public Behavior {
    regenny::ExActionState ex_action_state; // 0x830
    uint32_t health; // 0x858
    char pad_85c[0x84];
    regenny::ExWaypoint ex_waypoint; // 0x8e0
    char pad_990[0x108];
    regenny::ExNpc ex_npc; // 0xa98
    char pad_ad0[0xf0];
    uint32_t wait_flags; // 0xbc0
    char pad_bc4[0x8];
    uint32_t talk_flags; // 0xbcc
    char pad_bd0[0x70];
    float anim_spd_rate; // 0xc40
    char pad_c44[0xc];
    virtual ~BehaviorAppBase() = default;
    virtual void virtual_function_1() = 0;
    virtual void virtual_function_2() = 0;
    virtual void virtual_function_3() = 0;
    virtual void virtual_function_4() = 0;
    virtual void virtual_function_5() = 0;
    virtual void virtual_function_6() = 0;
    virtual void virtual_function_7() = 0;
    virtual void virtual_function_8() = 0;
    virtual void virtual_function_9() = 0;
    virtual void virtual_function_10() = 0;
    virtual void virtual_function_11() = 0;
    virtual void virtual_function_12() = 0;
    virtual void virtual_function_13() = 0;
    virtual void virtual_function_14() = 0;
    virtual void virtual_function_15() = 0;
    virtual void virtual_function_16() = 0;
    virtual void virtual_function_17() = 0;
    virtual void virtual_function_18() = 0;
    virtual void virtual_function_19() = 0;
    virtual void virtual_function_20() = 0;
    virtual void virtual_function_21() = 0;
    virtual void virtual_function_22() = 0;
    virtual void virtual_function_23() = 0;
    virtual void virtual_function_24() = 0;
    virtual void virtual_function_25() = 0;
    virtual void virtual_function_26() = 0;
    virtual void virtual_function_27() = 0;
    virtual void virtual_function_28() = 0;
    virtual void virtual_function_29() = 0;
    virtual void virtual_function_30() = 0;
    virtual void virtual_function_31() = 0;
    virtual void virtual_function_32() = 0;
    virtual void virtual_function_33() = 0;
    virtual void virtual_function_34() = 0;
    virtual void virtual_function_35() = 0;
    virtual void virtual_function_36() = 0;
    virtual void virtual_function_37() = 0;
    virtual void virtual_function_38() = 0;
    virtual void virtual_function_39() = 0;
    virtual void virtual_function_40() = 0;
    virtual void virtual_function_41() = 0;
    virtual void virtual_function_42() = 0;
    virtual void virtual_function_43() = 0;
    virtual void virtual_function_44() = 0;
    virtual void virtual_function_45() = 0;
    virtual void virtual_function_46() = 0;
    virtual void virtual_function_47() = 0;
    virtual void virtual_function_48() = 0;
    virtual void virtual_function_49() = 0;
    virtual void virtual_function_50() = 0;
    virtual void virtual_function_51() = 0;
    virtual void virtual_function_52() = 0;
    virtual void virtual_function_53() = 0;
    virtual void virtual_function_54() = 0;
    virtual void virtual_function_55() = 0;
    virtual void virtual_function_56() = 0;
    virtual void virtual_function_57() = 0;
    virtual void virtual_function_58() = 0;
    virtual void virtual_function_59() = 0;
    virtual void virtual_function_60() = 0;
    virtual void virtual_function_61() = 0;
    virtual void virtual_function_62() = 0;
    virtual void virtual_function_63() = 0;
    virtual void virtual_function_64() = 0;
    virtual void virtual_function_65() = 0;
    virtual void virtual_function_66() = 0;
    virtual void virtual_function_67() = 0;
    virtual void virtual_function_68() = 0;
    virtual void virtual_function_69() = 0;
    virtual void virtual_function_70() = 0;
    virtual void virtual_function_71() = 0;
    virtual void virtual_function_72() = 0;
    virtual void virtual_function_73() = 0;
    virtual void virtual_function_74() = 0;
    virtual void virtual_function_75() = 0;
    virtual void virtual_function_76() = 0;
    virtual void virtual_function_77() = 0;
    virtual void virtual_function_78() = 0;
    virtual void virtual_function_79() = 0;
    virtual void virtual_function_80() = 0;
    virtual void virtual_function_81() = 0;
    virtual void virtual_function_82() = 0;
    virtual void virtual_function_83() = 0;
    virtual void virtual_function_84() = 0;
    virtual void virtual_function_85() = 0;
    virtual void virtual_function_86() = 0;
    virtual void virtual_function_87() = 0;
    virtual void virtual_function_88() = 0;
    virtual void virtual_function_89() = 0;
    virtual void virtual_function_90() = 0;
    virtual void virtual_function_91() = 0;
    virtual void virtual_function_92() = 0;
    virtual void virtual_function_93() = 0;
    virtual void virtual_function_94() = 0;
    virtual void virtual_function_95() = 0;
    virtual void virtual_function_96() = 0;
    virtual void virtual_function_97() = 0;
    virtual void virtual_function_98() = 0;
    virtual void virtual_function_99() = 0;
    virtual void virtual_function_100() = 0;
    virtual void virtual_function_101() = 0;
    virtual void virtual_function_102() = 0;
    virtual void virtual_function_103() = 0;
    virtual void virtual_function_104() = 0;
    virtual void virtual_function_105() = 0;
    virtual void virtual_function_106() = 0;
    virtual void virtual_function_107() = 0;
    virtual void virtual_function_108() = 0;
    virtual void virtual_function_109() = 0;
    virtual void virtual_function_110() = 0;
    virtual void virtual_function_111() = 0;
    virtual void request_anim(uint32_t anim) = 0;
    virtual void request_anim_table(uint32_t anim) = 0;
    virtual void virtual_function_114() = 0;
    virtual void request_anim_op(void* a1, void* a2) = 0;
    virtual void request_move(regenny::Vector4f* pos, int32_t unk) = 0;
}; // Size: 0xc50
#pragma pack(pop)
}
