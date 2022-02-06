#include "AsyncJson.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

class WebServer {
   private:
    static void handleNotFound(AsyncWebServerRequest *request);
    static AsyncWebServer *server;
    static String accessKey;

   public:
    static void init(int port);
    static void on(const char *uri, WebRequestMethodComposite method,
                   ArRequestHandlerFunction onRequest);
    static void on(const char *uri, WebRequestMethodComposite method,
                   ArJsonRequestHandlerFunction onRequest);
    static void onAuthorized(const char *uri, WebRequestMethodComposite method,
                             ArRequestHandlerFunction onRequest);
    static void onAuthorized(const char *uri, WebRequestMethodComposite method,
                             ArJsonRequestHandlerFunction onRequest);
    static void setAccessKey(String accessKey);
    static boolean isAuthorized(AsyncWebServerRequest *request);
    static void addHandler(AsyncCallbackJsonWebHandler *handler);
    static void OK(AsyncWebServerRequest *request);
    static void badRequest(AsyncWebServerRequest *request);
    static void unauthorized(AsyncWebServerRequest *request);
    static void send(int code, char *contentType, const String &content);
    static int args();
    static String arg(int argIndex);
};