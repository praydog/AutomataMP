#pragma once
#include "cHitPrim.hpp"
namespace regenny {
#pragma pack(push, 1)
struct cObjHit : public cHitPrim {
    char pad_150[0x140];
}; // Size: 0x290
#pragma pack(pop)
}
