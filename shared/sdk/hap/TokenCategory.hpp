#pragma once

#include <cstdint>
#include <string_view>
#include "../lib/Noncopyable.hpp"

namespace sdk::hap {
class TokenCategory : public ::sdk::lib::Noncopyable {
public:
    static TokenCategory* get_first();
    static TokenCategory* find(std::string_view name);

public:
    virtual void unk_virtual0(void* a2, uint32_t a3) = 0;
    virtual void unk_virtual1() = 0;
    virtual void unk_virtual2() = 0;
    virtual ~TokenCategory(){};
    virtual int32_t unk_virtual4(void* a2) = 0;

public:
    uint32_t hash() const {
        return m_crc;
    }

    std::string_view category_name() const {
        return m_name;
    }

    TokenCategory* next_category() const {
        return m_next;
    }

private:
    uint32_t m_crc;
    uint32_t m_unk;
    const char* m_name;
    TokenCategory* m_next;
};

static_assert(sizeof(TokenCategory) == 0x20, "Size of TokenCategory is not correct.");
}