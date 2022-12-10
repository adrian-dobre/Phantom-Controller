#include "LightSensor.h"

MAX44009 LightSensor::sensor;
LightData LightSensor::data;

void readLightSensor(void *sensor) {
    while (true) {
        vTaskDelay(10000);
        LightSensor::data.lux = ((MAX44009 *)sensor)->get_lux();
    }
}

void LightSensor::init() {
    Wire.begin();
    sensor.begin();
    xTaskCreate(readLightSensor, "Read Light Sensor", 2048, &sensor, 1, NULL);
}
