#include <Arduino.h>

#include "DeviceConfiguration.h"
#include "NetworkConnection.h"
#include "RestApi.h"

void setup() {
    Serial.begin(115200);
    DeviceConfiguration::init();
    NetworkConnection::onConnected(RestApi::init);
    NetworkConnection::init();
}

void loop() {}