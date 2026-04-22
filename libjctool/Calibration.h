#pragma once
#include <cstdint>
#include "JoyConProtocol.h"

struct StickCalibration {
    int16_t minX, centerX, maxX;
    int16_t minY, centerY, maxY;
};

// Decode the 12-bit packed stick parameters
void jc_decode_stick_params(uint16_t* decoded, const uint8_t* encoded);

// Parse a 22-byte SPI calibration block into StickCalibration
StickCalibration jc_parse_stick_calibration(const uint8_t* spi_block);
