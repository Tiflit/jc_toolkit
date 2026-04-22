#include "Calibration.h"
#include "../jctool/jctool.h"   // for decode_stick_params()

void jc_decode_stick_params(uint16_t* decoded, const uint8_t* encoded) {
    // reuse existing implementation for now
    decode_stick_params(decoded, const_cast<uint8_t*>(encoded));
}

StickCalibration jc_parse_stick_calibration(const uint8_t* spi_block) {
    // First 9 bytes contain packed params for one stick (6 values)
    uint16_t decoded[6] = {};
    jc_decode_stick_params(decoded, spi_block);

    StickCalibration cal{};
    cal.minX    = decoded[0];
    cal.centerX = decoded[1];
    cal.maxX    = decoded[2];
    cal.minY    = decoded[3];
    cal.centerY = decoded[4];
    cal.maxY    = decoded[5];

    return cal;
}
