#include <utility/Crc32.hpp>
#include <spdlog/spdlog.h>

#include "ScriptFunctions.hpp"

ScriptFunctions& ScriptFunctions::get() {
    static ScriptFunctions scripts;
    return scripts;
}

ScriptFunctions::ScriptFunctions()
    : m_scriptFunctions((ScriptFunction**)0x141415930)
{

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
