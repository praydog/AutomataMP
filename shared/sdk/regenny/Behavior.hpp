#pragma once
#include "Obj.hpp"
namespace regenny {
#pragma pack(push, 1)
struct Behavior : public Obj {
    char pad_670[0x40];
    void* behavior_extension_array; // 0x6b0
    char pad_6b8[0x178];
    virtual ~Behavior() = default;
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
    virtual char start_animation(uint32_t anim, uint32_t variant, uint32_t a3, uint32_t a4) = 0;
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
    virtual void last_behavior_vfunc() = 0;
}; // Size: 0x830
#pragma pack(pop)
}
