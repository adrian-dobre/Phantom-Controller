#include "NetworkConnection.h"
void (*NetworkConnection::onConnectedCallback)(bool apMode);
void NetworkConnection::onConnected(void (*onConnectedCallback)(bool apMode)) {
    NetworkConnection::onConnectedCallback = onConnectedCallback;
}

void NetworkConnection::init() {
    DeviceConfiguration::init();
    WiFiConfiguration configuration =
        DeviceConfiguration::getWiFiConfiguration();

    bool hasCredentials = !configuration.ssid.isEmpty() && !configuration.password.isEmpty() &&
        !configuration.password.isEmpty();
    if (hasCredentials &&
        !DeviceConfiguration::doubleResetDetected) {
        startStation(configuration.ssid, configuration.password);
    } else {
        startAP(hasCredentials);
    }
}

void accessPointTimeout(void* param) {
    vTaskDelay(120000);
    if (WiFi.getMode() == WIFI_AP) {
        Serial.println("AP mode timeout. Resetting...");
        ESP.restart();
    }
}

void NetworkConnection::startAP(bool hasCredentials) {
    Serial.println("Starting in AP mode.");
    WiFi.mode(WIFI_AP);
    WiFi.softAP("Phantom Controller", "phantom-controller");
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());

    if (hasCredentials) {
        xTaskCreate(accessPointTimeout, "Access Point Timeout", 1024, NULL, 1,
                    NULL);
    }
    if (onConnectedCallback) {
        onConnectedCallback(true);
    }
}

void NetworkConnection::startStation(String ssid, String password) {
    Serial.print("Starting in station mode.");
    int stationTimeout = millis() + 60000;
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    while (WiFi.status() != WL_CONNECTED && millis() < stationTimeout) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    if (millis() > stationTimeout) {
        Serial.println("Station mode connect timeout.");
        startAP(true);
    } else {
        if (onConnectedCallback) {
            onConnectedCallback(false);
        }
    }
}
