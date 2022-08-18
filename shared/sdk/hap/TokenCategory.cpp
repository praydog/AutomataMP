#pragma once

#include <spdlog/spdlog.h>
#include <utility/Scan.hpp>
#include <utility/Module.hpp>

#include "TokenCategory.hpp"

namespace sdk::hap {
static TokenCategory* get_first() {
    static TokenCategory** first = []() -> TokenCategory** {
        spdlog::info("Finding first token category...");

        const auto string_ref = utility::scan_string(utility::get_executable(), "GlobalPhase");

        if (!string_ref) {
            spdlog::error("Failed to find first token category.");
            return nullptr;
        }

        const auto ref = utility::scan_disasm(*string_ref, 100, "48 89 ? ? ? ? ? 48 89 ? ? ? ? ?");

        if (!ref) {
            spdlog::error("Failed to find first token category.");
            return nullptr;
        }

        const auto result = (TokenCategory**)utility::calculate_absolute(*ref + 10);

        spdlog::info("Token category: {:x}", (uintptr_t)result);
    }();

    return *first;
}

static TokenCategory* find(std::string_view name) {
    const auto first = get_first();

    if (first == nullptr) {
        return nullptr;
    }

    for (auto category = first; category != nullptr; category = category->next_category()) {
        if (category->category_name() == name) {
            return category;
        }
    }

    return nullptr;
}
}