#include "StickUtils.h"

RawStick decode_raw_stick(const uint8_t* data) {
    RawStick s;
    s.x = static_cast<uint16_t>( data[0]         | ((data[1] & 0x0F) << 8) );
    s.y = static_cast<uint16_t>((data[1] >> 4)   |  (data[2]        << 4) );
    return s;
}

float normalize_axis(int raw, int min_val, int center, int max_val) {
    if (raw > center) {
        // Guard against degenerate calibration (max == center).
        if (max_val == center) return 1.0f;
        return static_cast<float>(raw - center) /
               static_cast<float>(max_val - center);
    }
    if (raw < center) {
        // Guard against degenerate calibration (min == center).
        if (min_val == center) return -1.0f;
        return static_cast<float>(raw - center) /
               static_cast<float>(center - min_val);
    }
    return 0.0f;
}
