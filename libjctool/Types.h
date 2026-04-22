#pragma once
#include <cstdint>

// ---------------------------------------------------------------------------
// Stick calibration: absolute axis limits computed from SPI data.
// All values are in raw 12-bit ADC space (0–4095).
// ---------------------------------------------------------------------------
struct StickCalibration {
    int16_t minX, centerX, maxX;
    int16_t minY, centerY, maxY;
};

// ---------------------------------------------------------------------------
// Normalized stick output: both axes in [-1.0, 1.0].
// ---------------------------------------------------------------------------
struct StickState {
    float x = 0.0f;
    float y = 0.0f;
};

// ---------------------------------------------------------------------------
// Raw 12-bit stick values straight from a HID input report.
// ---------------------------------------------------------------------------
struct RawStick {
    uint16_t x = 0;
    uint16_t y = 0;
};
