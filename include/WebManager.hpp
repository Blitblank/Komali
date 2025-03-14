
#pragma once

#include "ESPAsyncWebServer.h"

namespace wifi {

class WebManager {
    public:
    WebManager();
    ~WebManager() = default;
    
    static void taskWrapper(void * args);

    void setup();
    void run();

    void setTemp(double temp) { temperature = temp; }

    private:

    void scanNetworks();
    String onWebSocketMessage(void *arg, uint8_t *data, size_t len);
    void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    void tempSenderTask();
    void processCommand(String command);

    const char* ssid = "mcgee-2.4G";
    const char* password = "aiRey56v";
    AsyncWebServer server = AsyncWebServer(80);
    AsyncWebSocket ws = AsyncWebSocket("/ws");

    double temperature = 0.0;

};

}