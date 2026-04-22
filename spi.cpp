#include "Spi.h"
#include "../jctool/jctool.h"

// SPI wrappers
int jc_get_spi_data(uint32_t offset, uint16_t read_len, uint8_t* buffer) {
    return get_spi_data(offset, read_len, buffer);
}

int jc_write_spi_data(uint32_t offset, uint16_t write_len, uint8_t* buffer) {
    return write_spi_data(offset, write_len, buffer);
}

// Device info wrappers
int jc_get_device_info(uint8_t* buffer) {
    return get_device_info(buffer);
}

int jc_get_battery(uint8_t* buffer) {
    return get_battery(buffer);
}

int jc_get_temperature(uint8_t* buffer) {
    return get_temperature(buffer);
}

// Stick parameter wrappers
void jc_decode_stick_params(uint16_t* decoded, uint8_t* encoded) {
    decode_stick_params(decoded, encoded);
}

void jc_encode_stick_params(uint8_t* encoded, uint16_t* decoded) {
    encode_stick_params(encoded, decoded);
}
