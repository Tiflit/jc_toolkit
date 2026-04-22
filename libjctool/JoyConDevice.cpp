#include "Calibration.h"
#include "JoyConDevice.h"
#include "Spi.h"
#include "StickUtils.h"
#include "../jctool/jctool.h"   // temporary dependency, will be removed later

JoyConDevice::JoyConDevice() {
}

JoyConDevice::~JoyConDevice() {
    disconnect();
}

bool JoyConDevice::connect() {
    int res = device_connection();
    if (res == 0) {
        leftCal  = getLeftCalibration();
        rightCal = getRightCalibration();
        calLoaded = true;
        return true;
    }
    return false;
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

StickState JoyConDevice::computeStick(const StickCalibration& cal,
                                      uint16_t rawX,
                                      uint16_t rawY) const {
    StickState s{};
    if (!calLoaded) {
        s.x = 0.0f;
        s.y = 0.0f;
        return s;
    }

    s.x = normalize_axis(rawX, cal.minX, cal.centerX, cal.maxX);
    s.y = normalize_axis(rawY, cal.minY, cal.centerY, cal.maxY);

    if (s.x < -1.0f) s.x = -1.0f;
    if (s.x >  1.0f) s.x =  1.0f;
    if (s.y < -1.0f) s.y = -1.0f;
    if (s.y >  1.0f) s.y =  1.0f;

    return s;
}

StickState JoyConDevice::getLeftStick() {
    StickState s{};

    // TODO: replace this with real HID input report reading.
    // For now, this is just a placeholder to show how normalization is applied.
    uint8_t report[64] = {};
    // Example: assume left stick data starts at offset 6 in a 0x30 report.
    RawStick raw = decode_raw_stick(&report[6]);

    return computeStick(leftCal, raw.x, raw.y);
}

StickState JoyConDevice::getRightStick() {
    StickState s{};

    // TODO: replace this with real HID input report reading.
    uint8_t report[64] = {};
    // Example: assume right stick data starts at offset 9 in a 0x30 report.
    RawStick raw = decode_raw_stick(&report[9]);

    return computeStick(rightCal, raw.x, raw.y);
}
