#include <spdlog/spdlog.h>

#include <utility/Scan.hpp>
#include <utility/Module.hpp>

#include "Game.hpp"

namespace nier {
// not loading
bool isLoading() {
    //return *(uint8_t*)0x1414838E8 != 1; // 2017 version
    static bool* is_loading_ptr = []() ->bool* {
        spdlog::info("Searching for isLoading...");

        const auto func = utility::scan(utility::get_executable(), "48 83 ec 28 e8 ? ? ? ? c7 05 ? ? ? ? 01 00 00 00");

        if (!func) {
            spdlog::error("Failed to find isLoading.");
            return nullptr;
        }

        bool* result = (bool*)utility::calculate_absolute(*func + 11, 8);

        spdlog::info("isLoading: {:x}", (uintptr_t)result);

        return result;
    }();

    return !*is_loading_ptr;
}
}