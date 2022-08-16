#pragma once

#include <cstdint>
#include <string_view>

uint32_t crc32(uint8_t* data, size_t len);
uint32_t crc32(std::string_view str);