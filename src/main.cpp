
#include "Engine.hpp"

program::Engine engine;

void setup() {

	Serial.begin(115200);

	engine.init();
}

void loop() {

	engine.run();

}
