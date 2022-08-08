#pragma once

#include "regenny/Model.hpp"
#include "Math.hpp"
#include "Parts.hpp"

namespace sdk {
// cModel
class Model : public sdk::Parts {
public:
    virtual ~Model() = 0;
    virtual void model1(...) = 0;
    virtual void model2(...) = 0;

    ::regenny::Model* as_model() const {
        return (regenny::Model*)this;
    }

    Vector3f& position() {
        return *(Vector3f*)&as_model()->position;
    }

    float& facing() {
        return as_model()->facing;
    }

    auto& tick_count() {
        return as_model()->tick_count;
    }


private:
    // TODO: map out this structure
    uint8_t detail_modeldata[sizeof(::regenny::Model) - sizeof(void*)];
};

static_assert(sizeof(Model) == 0x590, "Size of cModel is not correct.");
}