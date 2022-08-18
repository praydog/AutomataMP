#pragma once

#include <string_view>

namespace sdk::hap {
class string_t {
public:
    std::string_view data() const {
        return m_data;
    }

private:
    const char* m_data{}; // might actually be a union?
};
}