#include "RestApi.h"

#include "ClimateSensor.h"
#include "Co2Sensor.h"
#include "DeviceConfiguration.h"
// #include "LightSensor.h"
#include "LivoloController.h"
// #include "PhantomController.h"
#include "GreeG10ACController.h"
#include "WebServer.h"

double round2(double value) { return (int)(value * 100 + 0.5) / 100.0; }

void RestApi::init(bool apMode) {
    ClimateSensor::init();
    Co2Sensor::init();
    // LightSensor::init();
    GreeG10ACController::init();
    LivoloController::init();
    WebServer::init(80);
    WiFiConfiguration wiFiConfiguration =
        DeviceConfiguration::getWiFiConfiguration();
    if (apMode) {
        Serial.println(
            "Server started in AP mode. Adding configuration portal.");
        WebServer::on(
            "/", HTTP_GET, [wiFiConfiguration](AsyncWebServerRequest *request) {
                request->send(SPIFFS, "/files/index.html", "text/html", false,
                              [wiFiConfiguration](const String &var) {
                                  if (var == "ssid") {
                                      return wiFiConfiguration.ssid;
                                  }
                                  if (var == "password") {
                                      return wiFiConfiguration.password;
                                  }
                                  if (var == "access-key") {
                                      return wiFiConfiguration.accessKey;
                                  }
                                  return String();
                              });
            });

        WebServer::on("/phantom-controller/config", HTTP_PUT,
                      [](AsyncWebServerRequest *request, JsonVariant &json) {
                          if (!json.isNull()) {
                              String ssid = json["ssid"] | "";
                              String password = json["password"] | "";
                              String accessKey = json["access-key"] | "";
                              if (!ssid.isEmpty() && !password.isEmpty() &&
                                  !accessKey.isEmpty()) {
                                  WiFiConfiguration configuration;
                                  configuration.ssid = ssid;
                                  configuration.password = password;
                                  configuration.accessKey = accessKey;
                                  DeviceConfiguration::saveWiFiConfiguration(
                                      configuration);
                                  WebServer::OK(request);
                                  return ESP.restart();
                              }
                          }
                          WebServer::badRequest(request);
                      });

        WebServer::on("/phantom-controller/reset", HTTP_POST,
                      [](AsyncWebServerRequest *request) {
                          DeviceConfiguration::reset();
                          WebServer::OK(request);
                          ESP.restart();
                      });

        WebServer::on("/phantom-controller/restart", HTTP_POST,
                      [](AsyncWebServerRequest *request) {
                          WebServer::OK(request);
                          ESP.restart();
                      });
    } else {
        Serial.println("Server started in Station mode. Setting up RestAPI");
        WebServer::setAccessKey(wiFiConfiguration.accessKey);
        WebServer::onAuthorized(
            "/phantom-controller/stats", HTTP_GET,
            [](AsyncWebServerRequest *request) {
                // LightData lightData = LightSensor::data;
                ClimateData climateData = ClimateSensor::data;
                StaticJsonDocument<256> doc;

                // doc["climate"]["light"] = lightData.lux;
                doc["climate"]["temperature"] = round2(climateData.temparature);
                doc["climate"]["pressure"] = round2(climateData.pressure);
                doc["climate"]["humidity"] = round2(climateData.humidity);
                doc["climate"]["co2"] = round2(Co2Sensor::data.ppm);
                // doc["ventilation"]["mode"] =
                // PhantomController::ventilationMode;
                // doc["ventilation"]["fanSpeed"] = PhantomController::fanSpeed;
                // doc["ventilation"]["humidityLevel"] =
                //     PhantomController::humidityLevel;

                String buffer;
                serializeJson(doc, buffer);
                request->send(200, "application/json", buffer);
            });

        // WebServer::onAuthorized(
        //     "/phantom-controller/ventilation/mode", HTTP_PUT,
        //     [](AsyncWebServerRequest *request, JsonVariant &json) {
        //         if (!json.isNull()) {
        //             int mode = json["mode"];
        //             if (mode != 0 &&
        //             PhantomController::changeVentilationMode(
        //                                  (VentilationMode)mode)) {
        //                 return WebServer::OK(request);
        //             }
        //         }
        //         WebServer::badRequest(request);
        //     });

        // WebServer::onAuthorized(
        //     "/phantom-controller/ventilation/fan-speed", HTTP_PUT,
        //     [](AsyncWebServerRequest *request, JsonVariant &json) {
        //         if (!json.isNull()) {
        //             int fanSpeed = json["fanSpeed"];
        //             if (fanSpeed != 0 &&
        //                 PhantomController::changeFanSpeed((FanSpeed)fanSpeed))
        //                 { return WebServer::OK(request);
        //             }
        //         }
        //         WebServer::badRequest(request);
        //     });

        // WebServer::onAuthorized(
        //     "/phantom-controller/ventilation/humidity-level", HTTP_PUT,
        //     [](AsyncWebServerRequest *request, JsonVariant &json) {
        //         if (!json.isNull()) {
        //             int humidityLevel = json["humidityLevel"];
        //             if (humidityLevel != 0 &&
        //                 PhantomController::changeHumidityLevel(
        //                     (HumidityLevel)humidityLevel)) {
        //                 return WebServer::OK(request);
        //             }
        //         }
        //         WebServer::badRequest(request);
        //     });

        // WebServer::onAuthorized("/phantom-controller/ventilation/filter-reset",
        //                         HTTP_POST, [](AsyncWebServerRequest *request)
        //                         {
        //                             PhantomController::resetFilter();
        //                             WebServer::OK(request);
        //                         });

        WebServer::onAuthorized(
            "/phantom-controller/hvac/send", HTTP_POST,
            [](AsyncWebServerRequest *request, JsonVariant &json) {
                if (!json.isNull()) {
                    int power = json["power"];
                    int mode = json["mode"];
                    int fanSpeed = json["fanSpeed"];
                    int temperature = json["temperature"];
                    bool turbo = json["turbo"];
                    GreeG10ACController::send(power, mode, fanSpeed, temperature, turbo);
                    return WebServer::OK(request);
                }
                WebServer::badRequest(request);
            });

        WebServer::onAuthorized(
            "/phantom-controller/livolo/send", HTTP_POST,
            [](AsyncWebServerRequest *request, JsonVariant &json) {
                if (!json.isNull()) {
                    unsigned short int remoteId = json["remoteId"];
                    byte keyCode = json["keyCode"];

                    LivoloController::send(remoteId, keyCode);
                    return WebServer::OK(request);
                }
                WebServer::badRequest(request);
            });
    }
}
