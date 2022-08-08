#pragma once
#include "Obj.hpp"
namespace regenny {
#pragma pack(push, 1)
struct Behavior : public Obj {
    char pad_670[0x1c0];
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
}; // Size: 0x830
#pragma pack(pop)
}
