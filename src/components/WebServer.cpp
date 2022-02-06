#include "WebServer.h"

#include <regex>

String WebServer::accessKey;
AsyncWebServer *WebServer::server;

void WebServer::init(int port) {
    SPIFFS.begin(true);
    server = new AsyncWebServer(port);
    server->onNotFound(handleNotFound);
    server->begin();
}

void WebServer::setAccessKey(String acceeeKey) {
    WebServer::accessKey = acceeeKey;
}

void WebServer::handleNotFound(AsyncWebServerRequest *request) {
    String message = "{\"status\":\"NOT_FOUND\",";
    message += "\"URI\":\"";
    message += request->url();
    message += "\",\"Method\":\"";
    message += request->methodToString();
    message += "\"}";
    request->send(404, "application/json", message);
}

void WebServer::OK(AsyncWebServerRequest *request) {
    request->send(200, "application/json", "{\"status\":\"OK\"}");
}

void WebServer::badRequest(AsyncWebServerRequest *request) {
    request->send(400, "application/json", "{\"status\":\"BAD_REQUEST\"}");
}

void WebServer::unauthorized(AsyncWebServerRequest *request) {
    request->send(401, "application/json", "{\"status\":\"UNAUTHORIZED\"}");
}

void WebServer::on(const char *uri, WebRequestMethodComposite method,
                   ArRequestHandlerFunction onRequest) {
    server->on(uri, method, onRequest);
}

void WebServer::on(const char *uri, WebRequestMethodComposite method,
                   ArJsonRequestHandlerFunction onRequest) {
    AsyncCallbackJsonWebHandler *handler =
        new AsyncCallbackJsonWebHandler(uri, onRequest);
    handler->setMethod(method);
    server->addHandler(handler);
}

void WebServer::onAuthorized(const char *uri, WebRequestMethodComposite method,
                             ArRequestHandlerFunction onRequest) {
    WebServer::on(uri, method, [onRequest](AsyncWebServerRequest *request) {
        if (!WebServer::isAuthorized(request)) {
            return WebServer::unauthorized(request);
        }
        onRequest(request);
    });
}

void WebServer::onAuthorized(const char *uri, WebRequestMethodComposite method,
                             ArJsonRequestHandlerFunction onRequest) {
    WebServer::on(
        uri, method,
        [onRequest](AsyncWebServerRequest *request, JsonVariant &json) {
            if (!WebServer::isAuthorized(request)) {
                return WebServer::unauthorized(request);
            }
            onRequest(request, json);
        });
}

boolean WebServer::isAuthorized(AsyncWebServerRequest *request) {
    bool authorized = false;
    if (request->hasHeader("Authorization")) {
        std::string authorization =
            request->getHeader("Authorization")->value().c_str();
        std::regex rgx("Bearer (.+)");
        std::smatch matches;
        if (std::regex_search(authorization, matches, rgx)) {
            authorized =
                strcmp(matches[1].str().c_str(), accessKey.c_str()) == 0;
        }
    }
    return authorized;
}