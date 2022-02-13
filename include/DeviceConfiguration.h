#pragma once
#include <Preferences.h>
#include <WiFi.h>

enum FanSpeed { fan_low = 1, fan_medium = 2, fan_high = 3 };
enum HumidityLevel { humidity_low = 1, humidity_medium = 2, humidity_high = 3 };
enum VentilationMode {
    mode_automatic = 1,
    mode_monitor = 2,
    mode_manual = 3,
    mode_night = 4,
    mode_insert = 5,
    mode_evacuation = 6,
    mode_masterSlave = 7,
    mode_slaveMaster = 8,
    mode_temporaryEvacuation = 9
};

struct WiFiConfiguration {
    String ssid;
    String password;
    String accessKey;
};

struct PhantomConfiguration {
    FanSpeed fanSpeed;
    HumidityLevel humidityLevel;
    VentilationMode ventilationMode;
};

class DeviceConfiguration {
   private:
    static bool alreadyInited;

   public:
    static bool doubleResetDetected;
    static Preferences preferences;
    static void init();
    static WiFiConfiguration getWiFiConfiguration();
    static PhantomConfiguration getPhantomConfiguration();
    static void saveWiFiConfiguration(WiFiConfiguration wiFiConfiguration);
    static void savePhantomFanSpeed(FanSpeed fanSpeed);
    static void savePhantomHumidityLevel(HumidityLevel humidityLevel);
    static void savePhantomVentilationMode(VentilationMode ventilationMode);
    static void reset();
};