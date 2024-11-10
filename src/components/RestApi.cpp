#include "RestApi.h"

#include "DeviceConfiguration.h"
#include "LivoloController.h"
#include "WebServer.h"

double round2(double value) { return (int)(value * 100 + 0.5) / 100.0; }

void RestApi::init(bool apMode) {
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

        WebServer::on("/controller/config", HTTP_PUT,
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

        WebServer::on("/controller/reset", HTTP_POST,
                      [](AsyncWebServerRequest *request) {
                          DeviceConfiguration::reset();
                          WebServer::OK(request);
                          ESP.restart();
                      });

        WebServer::on("/controller/restart", HTTP_POST,
                      [](AsyncWebServerRequest *request) {
                          WebServer::OK(request);
                          ESP.restart();
                      });
    } else {
        Serial.println("Server started in Station mode. Setting up RestAPI");
        WebServer::setAccessKey(wiFiConfiguration.accessKey);
        WebServer::onAuthorized(
            "/controller/livolo/send", HTTP_POST,
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
