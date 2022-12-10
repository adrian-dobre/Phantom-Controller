#include "ClimateSensor.h"

ClimateData ClimateSensor::data;
BlueDot_BME280 ClimateSensor::sensor;

void readClimateSensor(void *sensor) {
    while (true) {
        vTaskDelay(10000);
        ClimateSensor::data.temparature =
            ((BlueDot_BME280 *)sensor)->readTempC();
        ClimateSensor::data.humidity =
            ((BlueDot_BME280 *)sensor)->readHumidity();
        ClimateSensor::data.pressure =
            ((BlueDot_BME280 *)sensor)->readPressure();
    }
}

void ClimateSensor::init() {
    // Hardware SPI
    sensor.parameter.communication = 2;
    sensor.parameter.SPI_cs = 5;
    sensor.parameter.sensorMode = 0b11;
    sensor.parameter.IIRfilter = 0b100;
    sensor.parameter.humidOversampling = 0b101;
    sensor.parameter.tempOversampling = 0b101;
    sensor.parameter.pressOversampling = 0b101;
    sensor.init();
    xTaskCreate(readClimateSensor, "Read Climate Sensor", 2048, &sensor, 1,
                NULL);
}
