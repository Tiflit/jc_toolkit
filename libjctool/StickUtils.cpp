#include "StickUtils.h"

// Joy-Con 12-bit packed format:
// data[0] = X low 8 bits
// data[1] = X high 4 bits + Y low 4 bits
// data[2] = Y high 8 bits

RawStick decode_raw_stick(const uint8_t* data) {
    RawStick s;

    s.x =  (data[0] | ((data[1] & 0x0F) << 8));
    s.y =  ((data[1] >> 4) | (data[2] << 4));

    return s;
}
