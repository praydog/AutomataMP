#pragma once

#include <cstdint>
#include <string>
#include <type_traits>

namespace sdk {
struct ScriptFunction {
    uintptr_t vtable; // 0 - 8
    uint32_t crc; // 8 - 10
    ScriptFunction* next; // 10 - 18
    uintptr_t unk; // 18 - 20
    uintptr_t function; // 20 - 28
    uintptr_t unk2;
};

class ScriptFunctions {
public:
    static ScriptFunctions& get();

public:
    ScriptFunctions();

    ScriptFunction* find(const std::string& name);

private:
    ScriptFunction** m_scriptFunctions;
};
}

#define OBJECT_SCRIPT_FUNCTION(object_name, func_name, ret_type, ...) \
    template<class... Args> \
    auto func_name(Args&&... args) {\
        static auto func = (ret_type (*)(decltype(this), __VA_ARGS__))sdk::ScriptFunctions::get().find(#object_name "." #func_name)->function; \
        return func(this, std::forward<Args>(args)...); \
    }
