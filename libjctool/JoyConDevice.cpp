#include "Calibration.h"
#include "JoyConDevice.h"
#include "Spi.h"
#include "../jctool/jctool.h"   // temporary dependency, will be removed later

JoyConDevice::JoyConDevice() {
}

JoyConDevice::~JoyConDevice() {
    disconnect();
}

bool JoyConDevice::connect() {
    // Temporary: reuse the old device_connection() function
    int res = device_connection();
    return (res == 0);
}

void JoyConDevice::disconnect() {
    // TODO: close HID handle properly once extracted
}

bool JoyConDevice::isConnected() const {
    return check_connection_ok;
}

StickCalibration JoyConDevice::getLeftCalibration() {
    uint8_t buf[22] = {};
    jc_get_spi_data(0x8010, sizeof(buf), buf);

    return jc_parse_stick_calibration(buf);
}

StickCalibration JoyConDevice::getRightCalibration() {
    uint8_t buf[22] = {};
    jc_get_spi_data(0x801B, sizeof(buf), buf);

    return jc_parse_stick_calibration(buf);
}

StickState JoyConDevice::getLeftStick() {
    StickState s{};
    // TODO: implement raw->normalized conversion
    return s;
}

StickState JoyConDevice::getRightStick() {
    StickState s{};
    // TODO: implement raw->normalized conversion
    return s;
}
