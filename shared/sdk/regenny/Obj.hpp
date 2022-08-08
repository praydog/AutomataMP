#pragma once
#include "EModel.hpp"
#include "Model.hpp"
namespace regenny {
struct Entity;
}
namespace regenny {
#pragma pack(push, 1)
struct Obj : public Model {
    char pad_590[0x8];
    uint32_t obj_flags; // 0x598
    char pad_59c[0x1c];
    regenny::EModel model_index; // 0x5b8
    char pad_5bc[0x54];
    regenny::Entity* entity; // 0x610
    char xmlbinary_data[64]; // 0x618
    char pad_658[0x18];
}; // Size: 0x670
#pragma pack(pop)
}
