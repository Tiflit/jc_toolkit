#include "JoyConDevice.h"
#include "Calibration.h"
#include "Spi.h"
#include "StickUtils.h"

#include <hidapi/hidapi.h>
#include <algorithm>   // std::clamp (C++17)
#include <cstring>

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

JoyConDevice::JoyConDevice() = default;

JoyConDevice::~JoyConDevice() {
    disconnect();
}

// ---------------------------------------------------------------------------
// Connection management
// ---------------------------------------------------------------------------

bool JoyConDevice::connect() {
    if (handle_) disconnect();

    // Try left Joy-Con first, then right, then Pro Controller.
    const uint16_t pids[] = {
        kJoyConLeftPID,
        kJoyConRightPID,
        kProControllerPID,
    };

    for (uint16_t pid : pids) {
        handle_ = hid_open(kNintendoVID, pid, nullptr);
        if (handle_) break;
    }

    if (!handle_) return false;

    // Non-blocking reads; we supply our own timeout via hid_read_timeout.
    hid_set_nonblocking(handle_, 0);

    // Cache calibration so getLeftStick() / getRightStick() can normalize
    // without hitting SPI on every call.
    calLoaded_ = readCalibration(kSpiUserCalLeft,  leftCal_) &&
                 readCalibration(kSpiUserCalRight, rightCal_);

    return true;
}

void JoyConDevice::disconnect() {
    if (handle_) {
        hid_close(handle_);
        handle_    = nullptr;
        calLoaded_ = false;
        hasReport_ = false;
        timingByte_ = 0;
    }
}

bool JoyConDevice::isConnected() const {
    return handle_ != nullptr;
}

// ---------------------------------------------------------------------------
// Input polling
// ---------------------------------------------------------------------------

bool JoyConDevice::pollReport(int timeoutMs) {
    if (!handle_) return false;

    std::memset(lastReport_, 0, sizeof(lastReport_));
    int res = hid_read_timeout(handle_, lastReport_, sizeof(lastReport_), timeoutMs);
    if (res > 0) {
        hasReport_ = true;
        return true;
    }
    return false;
}

StickState JoyConDevice::getLeftStick() const {
    if (!hasReport_ || !calLoaded_) return {};

    // Standard full input report (0x30): left stick starts at byte 6.
    RawStick raw = decode_raw_stick(&lastReport_[6]);
    return computeStick(leftCal_, raw.x, raw.y);
}

StickState JoyConDevice::getRightStick() const {
    if (!hasReport_ || !calLoaded_) return {};

    // Standard full input report (0x30): right stick starts at byte 9.
    RawStick raw = decode_raw_stick(&lastReport_[9]);
    return computeStick(rightCal_, raw.x, raw.y);
}

// ---------------------------------------------------------------------------
// Calibration — read (public)
// ---------------------------------------------------------------------------

bool JoyConDevice::getLeftCalibration(StickCalibration& out) const {
    return readCalibration(kSpiUserCalLeft, out);
}

bool JoyConDevice::getRightCalibration(StickCalibration& out) const {
    return readCalibration(kSpiUserCalRight, out);
}

// ---------------------------------------------------------------------------
// Calibration — write (public)
// ---------------------------------------------------------------------------

bool JoyConDevice::writeLeftCalibration(const StickCalibration& cal) {
    if (!writeCalibration(kSpiUserCalLeft, cal)) return false;
    leftCal_ = cal;   // keep cache in sync
    return true;
}

bool JoyConDevice::writeRightCalibration(const StickCalibration& cal) {
    if (!writeCalibration(kSpiUserCalRight, cal)) return false;
    rightCal_ = cal;
    return true;
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

StickState JoyConDevice::computeStick(const StickCalibration& cal,
                                      uint16_t rawX,
                                      uint16_t rawY) const {
    StickState s;
    s.x = normalize_axis(rawX, cal.minX, cal.centerX, cal.maxX);
    s.y = normalize_axis(rawY, cal.minY, cal.centerY, cal.maxY);

    // Clamp to [-1, 1] — handles out-of-range raws from imperfect cal.
    s.x = std::clamp(s.x, -1.0f, 1.0f);
    s.y = std::clamp(s.y, -1.0f, 1.0f);
    return s;
}

bool JoyConDevice::readCalibration(uint32_t spiAddress,
                                   StickCalibration& out) const {
    if (!handle_) return false;

    uint8_t buf[kSpiUserCalSize] = {};
    // jc_get_spi_data needs to advance timingByte_; cast away const because
    // the timing counter is logically mutable (like a cache).
    auto& tb = const_cast<u8&>(timingByte_);
    auto* h  = const_cast<hid_device*>(handle_);

    if (jc_get_spi_data(h, tb, spiAddress, kSpiUserCalSize, buf) != 0)
        return false;

    out = jc_parse_stick_calibration(buf);
    return true;
}

bool JoyConDevice::writeCalibration(uint32_t spiAddress,
                                    const StickCalibration& cal) {
    if (!handle_) return false;

    uint8_t buf[kSpiUserCalSize] = {};
    jc_encode_stick_calibration(buf, cal);
    return jc_write_spi_data(handle_, timingByte_,
                             spiAddress, kSpiUserCalSize, buf) == 0;
}
