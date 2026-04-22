#include <JoyConDevice.h>
#include <cstdio>

int main() {
    JoyConDevice jc;

    printf("Connecting...\n");
    if (!jc.connect()) {
        printf("No Joy-Con found. Make sure it is paired via Bluetooth.\n");
        return 1;
    }
    printf("Connected.\n");

    StickCalibration leftCal;
    if (jc.getLeftCalibration(leftCal)) {
        printf("Left stick calibration:\n");
        printf("  X: min=%d  center=%d  max=%d\n",
               leftCal.minX, leftCal.centerX, leftCal.maxX);
        printf("  Y: min=%d  center=%d  max=%d\n",
               leftCal.minY, leftCal.centerY, leftCal.maxY);
    }

    printf("Reading stick for 5 seconds...\n");
    for (int i = 0; i < 300; ++i) {
        if (jc.pollReport()) {
            StickState s = jc.getLeftStick();
            printf("  L stick: x=%.3f  y=%.3f\r", s.x, s.y);
            fflush(stdout);
        }
    }
    printf("\nDone.\n");
    return 0;
}
