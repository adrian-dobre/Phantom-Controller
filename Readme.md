# Phantom Controller

## What's this about?

This is an ESP32 based Weather Station that can measure:
- CO2 levels (ppm)
- atmospheric pressure (bar)
- ambient light (lux)
- relative humidity
- temperature

combined with an IR remote for NovingAir Phantom [Wireless](https://ventilatie-recuperare.ro/produs/recuperator-de-caldura-novingair-phantom-wireless) or [Active](https://ventilatie-recuperare.ro/produs/recuperator-de-caldura-novingair-phantom-active/) Heat-Recovery Ventilation system.

It uses the following sensors:
- MHZ-19C CO2 sensor
- MAX44009 ambient light sensor
- BME 280 temperature, humidity and pressure sensor


## Why do it?

Well, due to a few of reasons:
1. NovingAir Phantom HRV system does not have a connected solution, just a [standard IR remote control](https://ventilatie-recuperare.ro/produs/telecomanda-tc-phantom-active/)
2. It makes sense to combine the "controller" with a Weather Station - this can allow creating automations without having to purchase separate sensors
3. I like controlling my devices from my phone. I used a general purpose, programmable RC (Broadlink RM4 mini) for this in the past, but I didn't quite like how everything looked/worked.

## What does it do?

It exposes a configuration page, when booting in WiFi AP mode (similar to ESP WiFi managers), where you configure WiFi SSID, Password and an Access Key used to authenticate RestAPI calls.
![Configuration](./resources/demo/ControllerConfig.png?raw=true)

Once in WiFi Station mode, it exposes a Rest API that allows controlling the HRV and seeing HRV status and climate data. You can check out the [available APIs here](./src/components/RestApi.cpp).
```
curl --location --request GET 'http://<host>:<port>/phantom-controller/stats' \
--header 'Authorization: Bearer <AccessKey>'

Response:
{
    "climate": {
        "light": 123,
        "temperature": 20.84,
        "pressure": 1010.37,
        "humidity": 42.81,
        "co2": 1137
    },
    "ventilation": {
        "mode": 1,
        "fanSpeed": 1,
        "humidityLevel": 1
    }
}
```

You can control the device via Rest API calls or use [the mobile application](https://github.com/adrian-dobre/Phantom-Controller-Mobile)

![App Demo](./resources/demo/app-demo.gif?raw=true)

## What parts do I need?

1. [ESP32 DevKit V1](https://www.banggood.com/Geekcreit-ESP32-WiFi+bluetooth-Development-Board-Ultra-Low-Power-Consumption-Dual-Cores-Pins-Unsoldered-p-1214159.html?cur_warehouse=CN) (or others, just make sure the pins are correctly mapped)
2. [A couple of IR LEDs](https://www.tme.eu/ro/details/tsal6100-as12z/diode-led-ir/vishay/) - I used two because the HRV IR receiver is not very sensitive and I want to increase the changes for the command to reach the sensor
3. A couple of [NPN transistors](https://www.tme.eu/ro/details/2n5551/tranzistori-tht-npn/diotec-semiconductor/) to drive the LEDs
4. A few resistors for the LEDs
5. [MHZ-19(C) CO2 sensor](https://www.banggood.com/MH-Z19-MH-Z19C-IR-Infrared-CO2-Sensor-Module-Carbon-Dioxide-Gas-Sensor-NDIR-for-CO2-Monitor-400-5000ppm-UART-PWM-Output-MH-Z19C-p-1788261.html?cur_warehouse=CN)
6. [MAX44009 ambient light sensor](https://www.sigmanortec.ro/Modul-lumina-ambientala-GY-49-MAX44009-4-pini-p136248887)
7. [BME280 sensor](https://www.banggood.com/CJMCU-280E-BME280-High-Precision-Atmospheric-Pressure-Sensor-Module-Board-p-1897807.html?rmmds=myorder&cur_warehouse=CN)

You can wire everything according to this schematic:

![Schematic](./resources/schematic/Schematic_PhantomController.svg?raw=true)

I also built a custom PCB, you can check out [Gerber files here](./resources/gerber/Gerber_PhantomController.zip). Or you can use the [easyEDA project from here](./resources/easyEDA/PhantomController).

**IMPORTANT NOTE**: It seems that the [BME280 sensor that I used](https://www.banggood.com/CJMCU-280E-BME280-High-Precision-Atmospheric-Pressure-Sensor-Module-Board-p-1897807.html?rmmds=myorder&cur_warehouse=CN) has a different pin layout than most other sensors on the market. Make sure you use the same sensor or adjust the PCB/connections to your own sensor. In general, please check all the connections match before soldering stuff :).
