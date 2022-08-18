#pragma once

#include "regenny/Model.hpp"
#include "Math.hpp"
#include "Parts.hpp"

namespace sdk {
// cModel
class Model : public sdk::Parts {
public:
    static constexpr std::string_view class_name() {
        return "class cModel";
    }

public:
    virtual ~Model() = 0;
    virtual void model1(...) = 0;
    virtual void model2(...) = 0;

    ::regenny::Model* regenny() const {
        return (regenny::Model*)this;
    }

    Vector3f& position() {
        return *(Vector3f*)&regenny()->position;
    }

    float& facing() {
        return regenny()->facing;
    }

    auto& tick_count() {
        return regenny()->tick_count;
    }


private:
    // TODO: map out this structure
    uint8_t detail_modeldata[sizeof(::regenny::Model) - sizeof(void*)];
};

static_assert(sizeof(Model) == 0x590, "Size of cModel is not correct.");
}