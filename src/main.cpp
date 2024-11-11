#include <Arduino.h>

#include "DeviceConfiguration.h"
#include "NetworkConnection.h"
#include "RestApi.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 32, &Wire, -1);
bool displayAvailable = false;
bool displayOn = true;
unsigned int lastDisconnect = 0;

void setup() {
    Serial.begin(115200);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("SSD1306 allocation failed"));
    } else {
        displayAvailable = true;
    }

    if (displayAvailable) {
        display.clearDisplay();
        display.display();
        display.setTextColor(WHITE);
        display.setTextWrap(false);
        display.setTextSize(2);
        display.setCursor(28, 8);
        display.println("LIVOLO");
        display.display();
    }

    DeviceConfiguration::init();
    NetworkConnection::onConnected(RestApi::init);
    NetworkConnection::init();

    if (displayAvailable) {
        display.clearDisplay();
        display.display();
        display.setTextSize(1);
        display.dim(true);
    }
}

void loop() {
    if (displayAvailable) {
        bool accessPointMode = WiFi.getMode() == WIFI_MODE_AP;
        bool isConnected = accessPointMode || WiFi.status() == WL_CONNECTED;
        if (accessPointMode || !isConnected || millis() - lastDisconnect < 60000) {
            if (!displayOn) {
                display.ssd1306_command(SSD1306_DISPLAYON);
                displayOn = true;
            }
            if (!isConnected) {
                lastDisconnect = millis();
            }
            String wifiMode = F("Mode: ");
            wifiMode.concat(accessPointMode ? "Access Point" : "Station");
            String wifiStatus = F("Status: ");
            wifiStatus.concat(isConnected ? "Connected" : "Not Connected");
            String wiFiIP = F("IP: ");
            display.clearDisplay();
            display.invertDisplay((accessPointMode || !isConnected) && (millis() / 500  % 2) == 0);
            wiFiIP.concat(accessPointMode ? WiFi.softAPIP().toString() : WiFi.localIP().toString());
            display.setCursor(2, 1);
            display.println(wifiMode);
            display.setCursor(2, 12);
            display.println(wifiStatus);
            display.setCursor(2, 23);
            display.println(wiFiIP);
            display.display();    
        } else {
            if (displayOn) {
                display.ssd1306_command(SSD1306_DISPLAYOFF);
                displayOn = false;
            }
        }
    }
}

