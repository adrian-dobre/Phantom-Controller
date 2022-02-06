#include <MAX44009.h>
#include <Wire.h>

struct LightData {
    int lux;
};

class LightSensor {
   private:
    static MAX44009 sensor;

   public:
    static LightData data;
    static void init();
};