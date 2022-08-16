#pragma once

#include <cstdint>

#include <utility/RTTI.hpp>

#include "regenny/Obj.hpp"
#include "Model.hpp"

namespace sdk {
class Obj : public sdk::Model {
public:
    // Vtable starts at index 3
    virtual void obj3() = 0;

    __forceinline regenny::Obj* regenny() const {
        return (regenny::Obj*)this;
    }

    template <typename T>
    T* as() const {
        return (T*)this;
    }

    bool is(std::string_view name) const {
        return utility::rtti::derives_from(this, name);
    }

    bool is_pl0000() const {
        return is("class Pl0000");
    }

    bool is_behavior() const {
        return is("class Behavior");
    }

    bool is_behavior_appbase() const {
        return is("class BehaviorAppBase");
    }

    auto& obj_flags() {
        return regenny()->obj_flags;
    }
    
    auto& model_index() {
        return regenny()->model_index;
    }
private:
    uint8_t detail_obj_data[sizeof(::regenny::Obj) - sizeof(Model)];
};

static_assert(sizeof(Obj) == 0x670, "Size of cObj is not correct.");
}