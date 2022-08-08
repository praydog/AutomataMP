#include <spdlog/spdlog.h>

#include <utility/Scan.hpp>
#include <utility/Module.hpp>

#include "ScriptFunctions.hpp"
#include <sdk/Entity.hpp>

namespace sdk {
void Entity::assignAIRoutine(const std::string& name) {
    //auto realName = findExactString(GetModuleHandle(0), name);

    //if (realName) {
        // old 2017
        /*static void(*assignRoutine)(const char* name, EntityContainer*) = (decltype(assignRoutine))0x1400EFD70;
        assignRoutine(name.c_str(), this);

        *Address(this->entity).get(0x16478).as<uint32_t*>() = 1;*/

       static void(*assignRoutine)(const char* name, sdk::Entity*) = []() -> decltype(assignRoutine) {
            spdlog::info("[EntityContainer] Searching for assignRoutine...");

            const auto str = utility::scan_string(utility::get_executable(), "buddy_pascal");

            if (!str) {
                spdlog::error("[EntityContainer] Failed to find assignRoutine.");
                return nullptr;
            }

            const auto str_ref = utility::scan_reference(utility::get_executable(), *str);

            if (!str_ref) {
                spdlog::error("[EntityContainer] Failed to find assignRoutine.");
                return nullptr;
            }

            const auto ref = utility::scan_opcode(*str_ref, 10, 0xE9);

            if (!ref) {
                spdlog::error("[EntityContainer] Failed to find assignRoutine.");
                return nullptr;
            }

            const auto result = (decltype(assignRoutine))utility::calculate_absolute(*ref + 1);

            spdlog::info("[EntityContainer] assignRoutine: {:x}", (uintptr_t)result);

            return result;
       }();

       assignRoutine(name.c_str(), this);
       *Address(this->behavior).get(0x16488).as<uint32_t*>() = 1;
    //}
}
}