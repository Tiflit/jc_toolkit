#pragma once
#include <cstdint>
#include "JoyConProtocol.h"

// SPI read/write interface (temporary wrappers)
int jc_get_spi_data(uint32_t offset, uint16_t read_len, uint8_t* buffer);
int jc_write_spi_data(uint32_t offset, uint16_t write_len, uint8_t* buffer);

// Device info
int jc_get_device_info(uint8_t* buffer);
int jc_get_battery(uint8_t* buffer);
int jc_get_temperature(uint8_t* buffer);

// Stick parameter encoding/decoding
void jc_decode_stick_params(uint16_t* decoded, uint8_t* encoded);
void jc_encode_stick_params(uint8_t* encoded, uint16_t* decoded);
