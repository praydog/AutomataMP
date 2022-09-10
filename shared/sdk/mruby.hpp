#pragma once

#include <string_view>

#include "mruby.h"
#include "mruby/string.h"

namespace sdk::mruby {
mrb_state* get_state();
mrb_value eval(std::string_view code);
}