
#include "Engine.hpp"

#include "WiFi.h"
#include "AsyncTCP.h"

program::Engine::Engine() {

}

void program::Engine::init() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

	delay(500);

	Serial.println("Starting...");

	pinMode(2, OUTPUT);
	digitalWrite(2, HIGH);

	Serial.println("Setting up Sensor...");
	sensor.setup();

	Serial.println("Setting up WiFi...");
	wifi.setup();

	// send temperature every second
	xTaskCreate(&wifi::WebManager::taskWrapper, "TempSender", 2048, &wifi, 1, NULL);

	Serial.println("Setup Complete.");
	digitalWrite(2, LOW);
}

void program::Engine::run() {

    // everything else gets handled by the websocket events

	// test
	wifi.run();

    //Serial.println(sensor.readData());
    wifi.setTemp(sensor.readData());

}
 