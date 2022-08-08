#pragma once

#include "regenny/BehaviorAppBase.hpp"
#include "Behavior.hpp"

namespace sdk {
class BehaviorAppBase : public sdk::Behavior {
public:
    __forceinline ::regenny::BehaviorAppBase* as_behavior_app_base() const {
        return (::regenny::BehaviorAppBase*)this;
    }

    __forceinline uint32_t& health() {
        return as_behavior_app_base()->health;
    }

private:
    uint8_t detail_behavior_app_base_data[sizeof(::regenny::BehaviorAppBase) - sizeof(sdk::Behavior)];
};

static_assert(sizeof(BehaviorAppBase) == 0xC50, "Size of BehaviorAppBase is not correct.");
}