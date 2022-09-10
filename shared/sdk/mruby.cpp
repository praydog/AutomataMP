#include <spdlog/spdlog.h>

#include <utility/Scan.hpp>
#include <utility/Module.hpp>

#include "mruby.hpp"

// fix extern compile error
extern "C" const uint8_t mrblib_irep[1]{0};

namespace sdk {
namespace mruby {
// Alternate string ref: "$script%x"
mrb_state* get_state() {
    static mrb_state** state = []() -> mrb_state** {
        spdlog::info("Searching for mruby state...");

        // relevant strings: "alive?"
        /*
        48 8D 0D 33 86 C7 00                          lea     rcx, g_mruby ; this is NOT the state, it's at g_mruby + 8
        E8 AE 01 00 00                                call    create_and_setup_mruby
        48 89 47 08                                   mov     [rdi+8], rax // THIS is the state
        48 85 C0                                      test    rax, rax
        0F 84 88 01 00 00                             jz      loc_7FF710AAAC17
        */
        const auto ref = utility::scan(utility::get_executable(), "48 8D 0D ? ? ? ? E8 ? ? ? ? 48 89 47 08 48 85 c0");

        if (!ref) {
            spdlog::error("Failed to find mruby state!");
            return nullptr;
        }

        spdlog::info("ref: {:x}", *ref);

        const auto mruby_global = utility::calculate_absolute(*ref + 3);

        spdlog::info("global: {:x}", mruby_global);

        const auto result = mruby_global + sizeof(void*);

        spdlog::info("state: {:x}", result);

        return (mrb_state**)result;
    }();

    return *state;
}

mrb_value eval(std::string_view code) {
    auto state = get_state();

    if (state == nullptr) {
        return mrb_nil_value();
    }

    return mrb_funcall(state, mrb_top_self(state), "eval", 1, mrb_str_new_cstr(state, code.data()));
}
}
}