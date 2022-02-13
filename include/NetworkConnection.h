#include "DeviceConfiguration.h"

class NetworkConnection {
   private:
    static void (*onConnectedCallback)(bool apMode);
    static void startAP(bool hasCredentials = false);
    static void startStation(String ssid, String passsword);

   public:
    static void init();
    static void onConnected(void (*onConnectedCallback)(bool apMode));
};