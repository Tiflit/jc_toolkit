#pragma once
#include <cstdint>
#include "Types.h"

// ---------------------------------------------------------------------------
// Parse the 9-byte SPI calibration block for one stick into a
// StickCalibration with absolute min/center/max values.
//
// The SPI block encodes six 12-bit values in order:
//   [0] max X above center (positive offset)
//   [1] max Y above center (positive offset)
//   [2] center X
//   [3] center Y
//   [4] max X below center (positive offset, subtracted from center)
//   [5] max Y below center (positive offset, subtracted from center)
//
// spi_block must point to at least 9 bytes.
// ---------------------------------------------------------------------------
StickCalibration jc_parse_stick_calibration(const uint8_t* spi_block);

// ---------------------------------------------------------------------------
// Build the 9-byte SPI block from an absolute StickCalibration.
// encoded must point to at least 9 bytes.
// ---------------------------------------------------------------------------
void jc_encode_stick_calibration(uint8_t* encoded,
                                 const StickCalibration& cal);
