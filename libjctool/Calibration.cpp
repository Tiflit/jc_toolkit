#include "Calibration.h"
#include "Spi.h"   // jc_decode_stick_params / jc_encode_stick_params

// ---------------------------------------------------------------------------
// The Joy-Con user-calibration SPI block for one stick is 9 bytes.
// After decoding we get six 12-bit values:
//
//   decoded[0] = X range above center  (max_x - center_x, i.e. positive delta)
//   decoded[1] = Y range above center  (max_y - center_y)
//   decoded[2] = center X
//   decoded[3] = center Y
//   decoded[4] = X range below center  (center_x - min_x, i.e. positive delta)
//   decoded[5] = Y range below center  (center_y - min_y)
//
// Reference: dekuNukem/Nintendo_Switch_Reverse_Engineering
//            bluetooth_hid_subcommands_notes.md, SPI 0x8010 / 0x801B
// ---------------------------------------------------------------------------

StickCalibration jc_parse_stick_calibration(const uint8_t* spi_block) {
    uint16_t d[6] = {};
    jc_decode_stick_params(d, spi_block);

    StickCalibration cal{};
    cal.centerX = static_cast<int16_t>(d[2]);
    cal.centerY = static_cast<int16_t>(d[3]);
    cal.maxX    = cal.centerX + static_cast<int16_t>(d[0]);
    cal.maxY    = cal.centerY + static_cast<int16_t>(d[1]);
    cal.minX    = cal.centerX - static_cast<int16_t>(d[4]);
    cal.minY    = cal.centerY - static_cast<int16_t>(d[5]);
    return cal;
}

void jc_encode_stick_calibration(uint8_t* encoded,
                                 const StickCalibration& cal)
{
    uint16_t d[6] = {};
    d[0] = static_cast<uint16_t>(cal.maxX    - cal.centerX);  // X above center
    d[1] = static_cast<uint16_t>(cal.maxY    - cal.centerY);  // Y above center
    d[2] = static_cast<uint16_t>(cal.centerX);
    d[3] = static_cast<uint16_t>(cal.centerY);
    d[4] = static_cast<uint16_t>(cal.centerX - cal.minX);     // X below center
    d[5] = static_cast<uint16_t>(cal.centerY - cal.minY);     // Y below center
    jc_encode_stick_params(encoded, d);
}
