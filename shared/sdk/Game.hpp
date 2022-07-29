#pragma once

#include <cstdint>

namespace nier {
    // not loading
    bool isLoading() {
        return *(uint8_t*)0x1414838E8 != 1;
    }
}