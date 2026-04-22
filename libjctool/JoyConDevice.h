#pragma once
#include <cstdint>
#include <string>

struct StickCalibration {
    int16_t minX, centerX, maxX;
    int16_t minY, centerY, maxY;
};

struct StickState {
    float x; // normalized -1.0 to 1.0
    float y;
};

class JoyConDevice {
public:
    JoyConDevice();
    ~JoyConDevice();

    bool connect();
    void disconnect();
    bool isConnected() const;

    StickCalibration getLeftCalibration();
    StickCalibration getRightCalibration();

    StickState getLeftStick();
    StickState getRightStick();

private:
    StickCalibration leftCal{};
    StickCalibration rightCal{};
    bool calLoaded = false;

    StickState computeStick(const StickCalibration& cal,
                            uint16_t rawX,
                            uint16_t rawY) const;

    bool readInputReport(uint8_t* outReport, int timeoutMs = 5);
};
