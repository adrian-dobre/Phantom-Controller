# Phantom Controller

## What's this about?

This is an ESP32 based Weather Station that can measure:
- CO2 levels (ppm)
- atmospheric pressure (bar)
- ambient light (lux)
- relative humodity
- temperature

combined with an IR remote for NovingAir Phantom (Wireless) Heat-Recovery Ventilation system.

It uses the following sensors:
- MHZ-19C CO2 sensor
- MAX44009 ambient light sensor
- BME 280 temperature, humidity and pressure sensors


## Why do it?

Well, due to a couple of reasons:
1. NovingAir Phantom HRV system does not have a connected solution, just a standard IR remote control
2. It makes sense to combine the "controller" with a Weather Station - this can allow creating automations without having to purchase separate sensors
3. I like controlling my devices from my phone. I used a general purpose, programable RC (Broadlink RM4 mini) for this in the past, but I didn't quite like how everything looked/worked.

## What does it do?

It exposes a configuration page, when booting in WiFi AP mode (similar to ESP WiFi managers), where you configure WiFi SSID, Password and an Access Key used to authenticate RestAPI calls.

Once in WiFi Station mode, it exposes a Rest API that allows controlling the HRV and seeing HRV status and climate data. You can check out the [available APIs here](./src/components/RestApi.cpp).

You can control the device via Rest API calls or use the mobile application (coming soon)

## What parts do I need?

1. ESP32 DevKit V1 (or others, just make sure the pins are correctly mapped)
2. A couple of IR LEDs - I used two because the HRV IR receiver is not very sensitive and I want to increase the changes for the command to reach the sensor
3. A couple of NPN transistors to drive the LEDs
4. A few resistors for the LEDs
5. MHZ-19(C) CO2 sensor
6. MAX44009 ambient light sensor
7. BME280 sensor

You can wire everything according to this schematic:

I also build a custom PCB, you can check out Gerber files here.

**IMPORTANT NOTE**: It seems that the BME280 sensor that I used (link) has a different pin layout than most other sensors on the market. Make sure you use the same sensor or adjust the PCB/connections to your own sensor. In general, please check all the connections match before soldering stuff :).