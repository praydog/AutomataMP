#pragma once

#include <spdlog/spdlog.h>
#include <utility/Scan.hpp>
#include <utility/Module.hpp>

#include "TokenCategory.hpp"

namespace sdk::hap {
TokenCategory* TokenCategory::get_first() {
    static TokenCategory** first = []() -> TokenCategory** {
        spdlog::info("Finding first token category...");

        const auto str = utility::scan_string(utility::get_executable(), "GlobalPhase");

        if (!str) {
            spdlog::error("Failed to find first token category string.");
            return nullptr;
        }

        spdlog::info("Found first token category string at 0x{:X}.", *str);
        const auto string_ref = utility::scan_reference(utility::get_executable(), *str);

        if (!string_ref) {
            spdlog::error("Failed to find ref for first token category.");
            return nullptr;
        }

        spdlog::info("Found ref for first token category at 0x{:X}.", *string_ref);

        // start the scan at ref + 4 because that's where the next instruction lands and scan_disasm won't work otherwise.
        const auto ref = utility::scan_disasm(*string_ref + 4, 100, "48 89 ? ? ? ? ? 48 89 ? ? ? ? ?");

        if (!ref) {
            spdlog::error("Failed to find first token category.");
            return nullptr;
        }

        spdlog::info("Found first token category at 0x{:X}.", *ref);
        const auto result = (TokenCategory**)utility::calculate_absolute(*ref + 10);

        spdlog::info("Token category: {:x}", (uintptr_t)result);

        return result;
    }();

    return *first;
}

TokenCategory* TokenCategory::find(std::string_view name) {
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