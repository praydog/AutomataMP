#pragma once

#include <string_view>

namespace utility {
namespace rtti {
    bool derives_from(const void* obj, std::string_view type_name);
}
}