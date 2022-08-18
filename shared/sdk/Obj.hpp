#pragma once

#include <cstdint>

#include <utility/RTTI.hpp>

#include "regenny/Obj.hpp"
#include "NierRTTI.hpp"
#include "Model.hpp"

namespace sdk {
class Obj : public sdk::Model {
public:
    static constexpr std::string_view class_name() {
        return "class cObj";
    }

public:
    // Vtable starts at index 3
    virtual NierRTTI2* get_rtti() = 0;

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

    template<typename T>
    bool is() const {
        return utility::rtti::derives_from(this, T::class_name());
    }

    template<typename T>
    T* try_cast() {
        return this->is<T>() ? static_cast<T*>(this) : nullptr;
    }

    template<typename T>
    const T* try_cast() const {
        return this->is<const T>() ? static_cast<const T*>(this) : nullptr;
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

    bool is_em_base() const {
        return is("class EmBase");
    }

    bool is_npc() const {
        return is("class NPC");
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