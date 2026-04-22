#include "Calibration.h"
#include "../jctool/jctool.h"   // temporary, for decode_stick_params()

// Wrapper around the old decode_stick_params()
void jc_decode_stick_params(uint16_t* decoded, const uint8_t* encoded) {
    decode_stick_params(decoded, const_cast<uint8_t*>(encoded));
}

// Parse a 22-byte SPI calibration block
StickCalibration jc_parse_stick_calibration(const uint8_t* spi_block) {
    // spi_block layout:
    // [0..8]  = X/Y min/center/max (encoded)
    // [9..?]  = unused or right stick (depending on block)
    //
    // decode_stick_params expects 9 bytes → 6 decoded values

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
