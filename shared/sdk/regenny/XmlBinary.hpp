#pragma once
#include "Xml.hpp"
namespace regenny {
#pragma pack(push, 1)
struct XmlBinary : public Xml {
    char pad_8[0x38];
}; // Size: 0x40
#pragma pack(pop)
}
