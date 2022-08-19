#pragma once
#include "cModelExtendWorkBase.hpp"
namespace regenny {
#pragma pack(push, 1)
struct cModelExtendWork : public cModelExtendWorkBase {
    char pad_8[0x248];
}; // Size: 0x250
#pragma pack(pop)
}
