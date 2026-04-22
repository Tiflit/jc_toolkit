#include "Calibration.h"
#include "JoyConDevice.h"
#include "Spi.h"
#include "StickUtils.h"
#include "../jctool/jctool.h"   // temporary dependency, will be removed later

// If your project exposes the HID handle, declare it here.
// Adjust this extern to match your actual definition.
#include <hidapi/hidapi.h>
extern hid_device* handle;

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
    // TODO: close HID handle properly once extracted from legacy code
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

bool JoyConDevice::readInputReport(uint8_t* outReport, int timeoutMs) {
    if (!handle) {
        return false;
    }

    int res = hid_read_timeout(handle, outReport, 64, timeoutMs);
    return res > 0;
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
    uint8_t report[64] = {};
    if (!readInputReport(report)) {
        return StickState{0.0f, 0.0f};
    }

    // Left stick raw data starts at offset 6 in report 0x30
    RawStick raw = decode_raw_stick(&report[6]);
    return computeStick(leftCal, raw.x, raw.y);
}

StickState JoyConDevice::getRightStick() {
    uint8_t report[64] = {};
    if (!readInputReport(report)) {
        return StickState{0.0f, 0.0f};
    }

    // Right stick raw data starts at offset 9 in report 0x30
    RawStick raw = decode_raw_stick(&report[9]);
    return computeStick(rightCal, raw.x, raw.y);
}
