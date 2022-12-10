#include "Co2Sensor.h"

Co2Data Co2Sensor::data;
MHZ19 Co2Sensor::sensor;

void readCo2Sensor(void *sensor) {
    while (true) {
        vTaskDelay(10000);
        Co2Sensor::data.ppm = ((MHZ19 *)sensor)->getCO2();
    }
}

void Co2Sensor::init() {
    Serial2.begin(9600);
    sensor.begin(Serial2);
    sensor.autoCalibration(false);
    xTaskCreate(readCo2Sensor, "Read CO2 Sensor", 2048, &sensor, 1, NULL);
}