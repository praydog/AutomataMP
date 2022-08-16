#pragma once

#include <cstdint>

namespace sdk::hap {
class State {
public:
    uint32_t signal{};
    uint32_t unk{};
    uintptr_t unk2{};
    uint32_t unk3{};
};
}