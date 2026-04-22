#pragma once
#include <cstdint>
#include "Types.h"
#include "JoyConProtocol.h"

// Forward declaration — consumers of this header do not need hidapi directly.
struct hid_device_;
typedef struct hid_device_ hid_device;

// ---------------------------------------------------------------------------
// SPI addresses for user stick calibration.
// User calibration takes precedence over factory calibration when the magic
// bytes 0xA1B2 are present at the start of the user cal block.
// ---------------------------------------------------------------------------
static constexpr uint32_t kSpiUserCalLeft  = 0x8010;
static constexpr uint32_t kSpiUserCalRight = 0x801B;
static constexpr uint32_t kSpiUserCalSize  = 9;     // 6 × 12-bit values = 9 bytes

// ---------------------------------------------------------------------------
// Nintendo vendor / product IDs used for HID enumeration.
// ---------------------------------------------------------------------------
static constexpr uint16_t kNintendoVID    = 0x057E;
static constexpr uint16_t kJoyConLeftPID  = 0x2006;
static constexpr uint16_t kJoyConRightPID = 0x2007;
static constexpr uint16_t kProControllerPID = 0x2009;

// ---------------------------------------------------------------------------
// JoyConDevice
//
// Owns a single HID connection to one Joy-Con (left or right).
// All mutable protocol state lives here; there are no global variables.
// ---------------------------------------------------------------------------
class JoyConDevice {
public:
    JoyConDevice();
    ~JoyConDevice();

    // Non-copyable (owns a raw HID handle).
    JoyConDevice(const JoyConDevice&)            = delete;
    JoyConDevice& operator=(const JoyConDevice&) = delete;

    // -----------------------------------------------------------------------
    // Connection management
    // -----------------------------------------------------------------------

    // Opens the first matching Joy-Con HID device (left or right).
    // Reads and caches calibration from SPI on success.
    // Returns true on success.
    bool connect();

    // Closes the HID handle.
    void disconnect();

    // True when the HID handle is open.
    bool isConnected() const;

    // -----------------------------------------------------------------------
    // Input polling
    // -----------------------------------------------------------------------

    // Read one 0x30 standard full input report into the internal buffer.
    // Call this once per frame before reading stick values.
    // Returns true if a report was received within timeoutMs.
    bool pollReport(int timeoutMs = 5);

    // Parse left / right stick from the last polled report.
    // Returns {0, 0} if no report has been polled yet.
    StickState getLeftStick()  const;
    StickState getRightStick() const;

    // -----------------------------------------------------------------------
    // Calibration — read
    // -----------------------------------------------------------------------

    // Read the user calibration block from SPI for left / right stick.
    // Returns false if the SPI read fails.
    bool getLeftCalibration (StickCalibration& out) const;
    bool getRightCalibration(StickCalibration& out) const;

    // -----------------------------------------------------------------------
    // Calibration — write  (the core feature for TMR stick users)
    // -----------------------------------------------------------------------

    // Write a new user calibration block to SPI for left / right stick.
    // This permanently updates the controller's on-board calibration so
    // every host (Switch console, emulators, Steam) sees correct values.
    // Returns false if the SPI write fails.
    bool writeLeftCalibration (const StickCalibration& cal);
    bool writeRightCalibration(const StickCalibration& cal);

private:
    // HID handle — opened by connect(), closed by disconnect().
    hid_device* handle_    = nullptr;

    // Packet sequence counter (0x0–0xF, wraps).  Passed by reference to
    // Spi functions so they can advance it without needing global state.
    u8 timingByte_ = 0;

    // Cached calibration loaded at connect() time.
    StickCalibration leftCal_  = {};
    StickCalibration rightCal_ = {};
    bool calLoaded_            = false;

    // Last received full input report (64 bytes).
    uint8_t lastReport_[64] = {};
    bool    hasReport_      = false;

    // Internal helpers
    StickState computeStick(const StickCalibration& cal,
                            uint16_t rawX, uint16_t rawY) const;

    bool readCalibration(uint32_t spiAddress, StickCalibration& out) const;
    bool writeCalibration(uint32_t spiAddress, const StickCalibration& cal);
};
