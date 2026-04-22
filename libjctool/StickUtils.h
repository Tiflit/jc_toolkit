#pragma once
#include <cstdint>

struct RawStick {
    uint16_t x;
    uint16_t y;
};

// Decode 12-bit packed stick data from HID report
RawStick decode_raw_stick(const uint8_t* data);
