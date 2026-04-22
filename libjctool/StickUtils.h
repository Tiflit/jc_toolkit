#pragma once
#include <cstdint>
#include "Types.h"

// ---------------------------------------------------------------------------
// Unpack a 3-byte, 12-bit-per-axis block from a Joy-Con HID input report
// into separate 16-bit x/y values.
//
// Wire format (same for both sticks):
//   byte[0]  = X[7:0]
//   byte[1]  = Y[3:0] | X[11:8]  (low nibble = X high bits, high nibble = Y low bits)
//   byte[2]  = Y[11:4]
// ---------------------------------------------------------------------------
RawStick decode_raw_stick(const uint8_t* data);

// ---------------------------------------------------------------------------
// Map a raw 12-bit ADC sample onto [-1.0, 1.0] using the three calibration
// points for one axis.  Result is NOT clamped here; clamping is the
// caller's responsibility so it can choose how to handle out-of-range raws.
// ---------------------------------------------------------------------------
float normalize_axis(int raw, int min_val, int center, int max_val);
