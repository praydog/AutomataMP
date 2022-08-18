#pragma once

#include <string_view>

#include "../lib/Noncopyable.hpp"
#include "../NierRTTI.hpp"

namespace sdk::hap {
class StateObject : public ::sdk::lib::Noncopyable {
public:
    static StateObject* get_first();
    static StateObject* find(std::string_view name);

public:
    virtual ~StateObject(){};
    virtual ::sdk::NierRTTI* get_rtti() const = 0;

public:
    auto next_state() const {
        return m_next_state;
    }

    std::string_view state_name() const {
        return m_state_name;
    }

private:
    uint32_t m_unk_hash;
    void* m_unk_ptr;
    void* pad;
    const char* m_state_name;
    void* m_unk_ptr2;
    uint32_t m_crc2;
    ::sdk::hap::StateObject* m_next_state;
};

static_assert(sizeof(StateObject) == 0x40, "Size of StateObject is not correct.");
}