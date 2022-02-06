#include <map>

#include "DeviceConfiguration.h"

class PhantomController {
   private:
    static int address;
    static int filterReset;
    static int commandRepeats;
    static void sendCommand(int command);
    static void setupCodeMaps();
    static std::map<FanSpeed, int> fanSpeedCommandCodes;
    static std::map<HumidityLevel, int> humidityLevelCommandCodes;
    static std::map<VentilationMode, int> ventilationModeCommandCodes;

   public:
    static FanSpeed fanSpeed;
    static HumidityLevel humidityLevel;
    static VentilationMode ventilationMode;
    static void init();
    static bool changeFanSpeed(FanSpeed fanSpeed);
    static bool changeHumidityLevel(HumidityLevel humidityLevel);
    static bool changeVentilationMode(VentilationMode ventilationMode);
    static bool resetFilter();
};