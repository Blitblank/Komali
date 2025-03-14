
#include "Sensor.hpp"

hardware::Sensor::Sensor() {

}

void hardware::Sensor::setup() {
	if (!bmp.begin_I2C(BMP390_ADDRESS)) {
		Serial.println("Could not find a valid BMP3 sensor, check wiring!");
	}

	bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
	bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
	bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
	bmp.setOutputDataRate(BMP3_ODR_50_HZ);

    calibrate();
}

double hardware::Sensor::readData() {
    return bmp.readTemperature();
}

void hardware::Sensor::calibrate() {
    
}