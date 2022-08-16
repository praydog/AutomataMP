#include <utility/Crc32.hpp>
#include <spdlog/spdlog.h>

#include <utility/Scan.hpp>
#include <utility/Module.hpp>
#include <utility/RTTI.hpp>

#include "ScriptFunctions.hpp"

namespace sdk {
ScriptFunctions& ScriptFunctions::get() {
    static ScriptFunctions scripts;
    return scripts;
}

ScriptFunctions::ScriptFunctions()
    //: m_scriptFunctions((ScriptFunction**)0x141415930) // 2017 version
{
    /* string references
    7FF71082B867 + 0x16 EntityLayout.enableCreateEntity
    7FF71082B8D7 + 0x16 EntityLayout.isCreated
    7FF71082BA97 + 0x16 GroupImpl.isEnabled
    7FF71082BB07 + 0x16 GroupImpl.reset
    7FF71082BB77 + 0x16 GroupImpl.signal
    7FF71082BCC7 + 0x16 Hap.isEnabled
    7FF71082BD37 + 0x16 Hap.gotoIndex
    */

   m_scriptFunctions = []() -> decltype(m_scriptFunctions) {
        spdlog::info("Searching for script functions...");

        const auto str = utility::scan_string(utility::get_executable(), "EntityLayout.enableCreateEntity");

        if (!str) {
            spdlog::error("Failed to find script functions.");
            return nullptr;
        }

        const auto str_ref = utility::scan_reference(utility::get_executable(), *str);

        if (!str_ref) {
            spdlog::error("Failed to find script functions.");
            return nullptr;
        }

        const auto ref = utility::scan_disasm(*str_ref + 4, 10, "48 8B 05");

        if (!ref) {
            spdlog::error("Failed to find script functions.");
            return nullptr;
        }

        const auto result = utility::calculate_absolute(*ref + 3);

        spdlog::info("Script functions: {:x}", (uintptr_t)result);

        return (decltype(m_scriptFunctions))result;
   }();
   
   for (auto script = *m_scriptFunctions; script != nullptr; script = script->next) {
        const auto ti = utility::rtti::get_type_info(script);

        if (ti == nullptr) {
            continue;
        }

        spdlog::info("{}: {:x} {:x}", ti->name(), script->crc, script->function);
   }
}

ScriptFunction* ScriptFunctions::find(const std::string& name) {
    auto crc = crc32((uint8_t*)name.c_str(), name.length());

    for (auto script = *m_scriptFunctions; script != nullptr; script = script->next) {
        if (script->crc == crc) {
            spdlog::info("Found {} at {:x}, function {:x}", name.c_str(), (uintptr_t)script, (uintptr_t)script->function);
            return script;
        }

        if (script->next == *m_scriptFunctions) {
            break;
        }
    }

    return nullptr;
}
}