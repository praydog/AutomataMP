#pragma once

#include <cstdint>

namespace sdk {
struct NierRTTI {
    NierRTTI* next;
    NierRTTI* parent; // or base class, whatever. may not always be set to something, even if internally it has a base class.
    uint32_t unk1;
    uint32_t crc;
    uint32_t size;
    const char* name; // e.g. "hap::StateObject"
    union {
        void* sometimes_points_to_some_vtable; // e.g const app::layout_obj::StateGet::`vftable'
        int64_t sometimes_negative1;
    };

    NierRTTI* self;
};
}