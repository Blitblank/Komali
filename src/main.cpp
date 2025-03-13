
#include <Arduino.h>
#include <Adafruit_BMP3XX.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>

#define BMP390_ADDRESS 0x77

const char* ssid = "mcgee-2.4G";
const char* password = "aiRey56v";
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

Adafruit_BMP3XX bmp;

void setupSensor() {
	if (!bmp.begin_I2C(BMP390_ADDRESS)) {
		Serial.println("Could not find a valid BMP3 sensor, check wiring!");
	}

	bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
	bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
	bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
	bmp.setOutputDataRate(BMP3_ODR_50_HZ);
	
}

// WebSocket Event Handler
void onWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        String message = String((char*)data).substring(0, len);
        Serial.printf("Received WebSocket Message: %s\n", message.c_str());

        if (message == "get_temp") {
            float temp = bmp.readTemperature();
            String tempMessage = "{\"temperature\": " + String(temp) + "}";
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

// WebSocket Event Handler Registration
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, 
             void *arg, uint8_t *data, size_t len) {
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

void setupWifi() {
	// Connect to Wi-Fi

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
 
    // Delete the scan result to free memory for code below.
    WiFi.scanDelete();


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
	ws.onEvent(onEvent);
	server.addHandler(&ws);

	// Start Web Server
	server.begin();
}

void setup() {

	Serial.begin(115200);

	WiFi.mode(WIFI_STA);
    WiFi.disconnect();

	delay(500);

	Serial.println("Starting...");

	pinMode(2, OUTPUT);
	digitalWrite(2, HIGH);

	Serial.println("Setting up Sensor...");
	setupSensor();
	Serial.println("Setting up WiFi...");
	setupWifi();

	// send temperature every second
	xTaskCreate([](void *){
		while (true) {
			String message = "{\"temperature\": " + String(bmp.readTemperature()) + "}";
    		ws.textAll(message);
			Serial.println(message);
			vTaskDelay(pdMS_TO_TICKS(1000));  // 1-second delay
		}
	}, "TempSender", 2048, NULL, 1, NULL);

	Serial.println("Setup Complete.");
	digitalWrite(2, LOW);
}

void loop() {

	// everything else gets handled by the websocket events

	ws.cleanupClients();

}
