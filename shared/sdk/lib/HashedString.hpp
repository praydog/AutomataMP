#pragma once

#include <cstdint>
#include <string_view>

#include <utility/Crc32.hpp>

#include "../sys/StringSystem.hpp"

namespace sdk::lib {
template<typename Allocator = ::sdk::sys::StringSystem::Allocator>
class HashedString {
public:
    HashedString(std::string_view str) 
        : m_hash{crc32(str)} // PLACEHOLDER, IT MIGHT BE A DIFFERENT ALGORITHM.
    {

    }

    operator uint32_t() const {
        return m_hash;
    }
private:
    // PLACEHOLDER. IT MIGHT BE A DIFFERENT DATATYPE.
    uint32_t m_hash;
};
}