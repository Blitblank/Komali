
#pragma once

#include "Adafruit_BMP3XX.h"

#define BMP390_ADDRESS 0x77

namespace hardware {

class Sensor {
    public:
    Sensor();
    ~Sensor() = default;

    void setup();
    double readData();

    private:

    void calibrate();

    Adafruit_BMP3XX bmp;

    double calibrationOffset = 0.0;
    bool calibrated = false;

};

}