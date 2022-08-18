#pragma once

#include <string_view>
#include <type_traits>
#include <utility/Crc32.hpp>

namespace sdk::hap {
// A function like SceneStateSystem::has only seems to care about the CRC field.
// not sure about other ones.
struct SceneStateName {
    SceneStateName(std::string_view name) 
    {
        data = new(std::remove_pointer<decltype(data)>::type);
        data->crc = crc32(name);
    }

    ~SceneStateName() {
        delete data;
    }

    struct {
        char unk[0x20];
        uint32_t crc;
        char unk2[0x14];
        char name_maybe[128]; // its either embedded or a pointer if > a certain amount of bytes, not sure yet.
    } *data;
};

static_assert(sizeof(SceneStateName) == sizeof(void*), "SceneStateName is not the correct size.");
}