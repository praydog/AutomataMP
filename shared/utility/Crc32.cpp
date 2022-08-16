#include "Crc32.hpp"

uint32_t crc32(uint8_t* data, size_t len)
{
    uint32_t crc = 0xFFFFFFFF;

    while (len--) {
        crc ^= *data++;

        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (-int(crc & 1) & 0xEDB88320);
        }
    }

    return ~crc;
}

uint32_t crc32(std::string_view str) {
    return crc32((uint8_t*)str.data(), str.size());
}