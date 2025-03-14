
#include "WebManager.hpp"

wifi::WebManager::WebManager() {

}

void wifi::WebManager::scanNetworks() {
    int n = WiFi.scanNetworks();
    if(n == 0) {
        Serial.println("No networks found.");
        return;
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.printf("%2d",i + 1);
            Serial.print(" | ");
            Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
            Serial.print(" | ");
            Serial.printf("%4d", WiFi.RSSI(i));
            Serial.print(" | ");
            Serial.printf("%2d", WiFi.channel(i));
            Serial.print(" | ");
            switch (WiFi.encryptionType(i))
            {
            case WIFI_AUTH_OPEN:
                Serial.print("open");
                break;
            case WIFI_AUTH_WEP:
                Serial.print("WEP");
                break;
            case WIFI_AUTH_WPA_PSK:
                Serial.print("WPA");
                break;
            case WIFI_AUTH_WPA2_PSK:
                Serial.print("WPA2");
                break;
            case WIFI_AUTH_WPA_WPA2_PSK:
                Serial.print("WPA+WPA2");
                break;
            case WIFI_AUTH_WPA2_ENTERPRISE:
                Serial.print("WPA2-EAP");
                break;
            case WIFI_AUTH_WPA3_PSK:
                Serial.print("WPA3");
                break;
            case WIFI_AUTH_WPA2_WPA3_PSK:
                Serial.print("WPA2+WPA3");
                break;
            case WIFI_AUTH_WAPI_PSK:
                Serial.print("WAPI");
                break;
            default:
                Serial.print("unknown");
            }
            Serial.println();
            delay(10);
        }
    }
    Serial.println("");

    WiFi.scanDelete();
}

void wifi::WebManager::setup() {

    //scanNetworks();

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());

    // Configure WebSocket
    ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        this->onEvent(server, client, type, arg, data, len);
    });

    server.addHandler(&ws);

    // Start Web Server
    server.begin();
}

void wifi::WebManager::run() {
    ws.cleanupClients();
}

void wifi::WebManager::onWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        String message = String((char*)data).substring(0, len);
        Serial.printf("Received WebSocket Message: %s\n", message.c_str());

        if (message == "get_temp") {
            String tempMessage = "{\"temperature\": " + String(temperature) + "}";
            ws.textAll(tempMessage);  // Send to all clients
			return;
        }

        // LED Control
        if (message == "off") {
            digitalWrite(2, LOW);
        } else {
            digitalWrite(2, HIGH);
        }

        // Send response back to WebSocket client
        ws.textAll("LED set to: " + message);
    }
}

void wifi::WebManager::onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket Client #%u Connected\n", client->id());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket Client #%u Disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            onWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void wifi::WebManager::taskWrapper(void * args) {
    WebManager *instance = static_cast<WebManager *>(args);
    instance->tempSenderTask();
}

void wifi::WebManager::tempSenderTask() {
    while (true) {
        String message = "{\"temperature\": " + String(temperature) + "}";
        ws.textAll(message);
        Serial.println(message);
        vTaskDelay(pdMS_TO_TICKS(1000));  // 1-second delay
    }
}
