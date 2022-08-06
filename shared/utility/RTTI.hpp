#pragma once

#include <string>

namespace utility {
namespace rtti {
    bool derives_from(const void* obj, const std::string& type_name);
}
}