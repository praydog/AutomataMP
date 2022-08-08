#pragma once

#include <cstdint>
#include "Xml.hpp"

namespace sdk {
// cXmlBinary
class XmlBinary : public Xml {
public:

private:
    uint8_t detail_xmlbinary_data[0x40 - sizeof(Xml)];
};

static_assert(sizeof(XmlBinary) == 0x40, "Size of cXmlBinary is not correct.");
}