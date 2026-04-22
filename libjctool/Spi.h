#pragma once
#include <cstdint>
#include "JoyConProtocol.h"

// ---------------------------------------------------------------------------
// Forward declaration — the HID handle is owned by JoyConDevice.
// These functions take it as an explicit parameter so there is no global
// state dependency.
// ---------------------------------------------------------------------------
struct hid_device_;
typedef struct hid_device_ hid_device;

// ---------------------------------------------------------------------------
// SPI flash access.
// Returns 0 on success, negative on failure.
// ---------------------------------------------------------------------------
int jc_get_spi_data (hid_device* handle, u8& timing_byte,
                     uint32_t offset, uint16_t read_len,  uint8_t* buffer);

int jc_write_spi_data(hid_device* handle, u8& timing_byte,
                      uint32_t offset, uint16_t write_len, const uint8_t* buffer);

// ---------------------------------------------------------------------------
// Device info helpers.
// ---------------------------------------------------------------------------
int jc_get_device_info (hid_device* handle, u8& timing_byte, uint8_t* buffer);
int jc_get_battery     (hid_device* handle, u8& timing_byte, uint8_t* buffer);
int jc_get_temperature (hid_device* handle, u8& timing_byte, uint8_t* buffer);

// ---------------------------------------------------------------------------
// 12-bit packed stick parameter encoding / decoding.
//
// The Joy-Con SPI calibration block stores six 12-bit values packed into
// nine bytes.  Layout (little-endian nibbles):
//
//   byte 0        = val0[ 7:0]
//   byte 1 [3:0]  = val0[11:8]
//   byte 1 [7:4]  = val1[ 3:0]
//   byte 2        = val1[11:4]
//   ... and so on for val2–val5.
//
// decoded[6] must point to an array of at least 6 uint16_t.
// encoded[9] must point to an array of at least 9 uint8_t.
// ---------------------------------------------------------------------------
void jc_decode_stick_params(uint16_t* decoded, const uint8_t* encoded);
void jc_encode_stick_params(uint8_t*  encoded, const uint16_t* decoded);
