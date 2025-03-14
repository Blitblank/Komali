
#pragma once

#include "WebManager.hpp"
#include "Sensor.hpp"

namespace program {

class Engine {
    public:
    Engine();
    ~Engine() = default;

    void init();
    void run();

    private:

    wifi::WebManager wifi;

    hardware::Sensor sensor;

};

}