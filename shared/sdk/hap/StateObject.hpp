#pragma once

#include "../lib/Noncopyable.hpp"
#include "../NierRTTI.hpp"

namespace sdk::hap {
class StateObject : public ::sdk::lib::Noncopyable {
public:
    virtual ~StateObject(){};
    virtual ::sdk::NierRTTI* get_rtti() const = 0;

public:

private:
    uint8_t detail_stateobject_data[0x40 - sizeof(void*) - sizeof(::sdk::lib::Noncopyable)];
};

static_assert(sizeof(StateObject) == 0x40, "Size of StateObject is not correct.");
}