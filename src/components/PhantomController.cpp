#include "PhantomController.h"

#include <IRremote.hpp>
#include <queue>

int PhantomController::address = 0x69;
int PhantomController::filterReset = 0x40;
int PhantomController::commandRepeats = 0;

FanSpeed PhantomController::fanSpeed = FanSpeed::fan_low;
HumidityLevel PhantomController::humidityLevel = HumidityLevel::humidity_low;
VentilationMode PhantomController::ventilationMode =
    VentilationMode::mode_automatic;

std::map<FanSpeed, int> PhantomController::fanSpeedCommandCodes;
std::map<HumidityLevel, int> PhantomController::humidityLevelCommandCodes;
std::map<VentilationMode, int> PhantomController::ventilationModeCommandCodes;

void PhantomController::setupCodeMaps() {
    if (humidityLevelCommandCodes.empty()) {
        humidityLevelCommandCodes.insert(
            std::make_pair(HumidityLevel::humidity_low, 0x4A));
        humidityLevelCommandCodes.insert(
            std::make_pair(HumidityLevel::humidity_medium, 0x4B));
        humidityLevelCommandCodes.insert(
            std::make_pair(HumidityLevel::humidity_high, 0x4C));

        fanSpeedCommandCodes.insert(std::make_pair(FanSpeed::fan_low, 0x6C));
        fanSpeedCommandCodes.insert(std::make_pair(FanSpeed::fan_medium, 0x6D));
        fanSpeedCommandCodes.insert(std::make_pair(FanSpeed::fan_high, 0x6E));

        ventilationModeCommandCodes.insert(
            std::make_pair(VentilationMode::mode_automatic, 0x28));
        ventilationModeCommandCodes.insert(
            std::make_pair(VentilationMode::mode_monitor, 0x29));
        ventilationModeCommandCodes.insert(
            std::make_pair(VentilationMode::mode_manual, 0x2A));
        ventilationModeCommandCodes.insert(
            std::make_pair(VentilationMode::mode_night, 0x4F));
        ventilationModeCommandCodes.insert(
            std::make_pair(VentilationMode::mode_insert, 0x2D));
        ventilationModeCommandCodes.insert(
            std::make_pair(VentilationMode::mode_evacuation, 0x2C));
        ventilationModeCommandCodes.insert(
            std::make_pair(VentilationMode::mode_masterSlave, 0x2E));
        ventilationModeCommandCodes.insert(
            std::make_pair(VentilationMode::mode_slaveMaster, 0x2F));
        ventilationModeCommandCodes.insert(
            std::make_pair(VentilationMode::mode_temporaryEvacuation, 0x48));
    }
}

std::queue<int> commandsQueue = std::queue<int>();
unsigned long int tempEvacStartTime = 0;
VentilationMode beforeTempEvacMode = VentilationMode::mode_automatic;

void resetTempEvacIfTimedOut() {
    if (PhantomController::ventilationMode ==
            VentilationMode::mode_temporaryEvacuation &&
        millis() - tempEvacStartTime > 1200000) {
        PhantomController::ventilationMode = beforeTempEvacMode;
    }
}

void setupTempEvac() {
    if (PhantomController::ventilationMode !=
        VentilationMode::mode_temporaryEvacuation) {
        beforeTempEvacMode = PhantomController::ventilationMode;
    }
    tempEvacStartTime = millis();
}

void asyncCommandProcessor(void *arg) {
    while (true) {
        resetTempEvacIfTimedOut();
        if (!commandsQueue.empty()) {
            int commandToSend = commandsQueue.front();
            commandsQueue.pop();
            // increase the likelyhood the HRV receives the command
            int repeatSend = 4;
            while (repeatSend > 0) {
                IrSender.sendNEC(PhantomController::address, commandToSend,
                                 PhantomController::commandRepeats);
                repeatSend--;
                vTaskDelay(125);
            }
        }
        vTaskDelay(500);
    }
}

void PhantomController::init() {
    setupCodeMaps();
    DeviceConfiguration::init();
    PhantomConfiguration phantomConfiguration =
        DeviceConfiguration::getPhantomConfiguration();
    fanSpeed = phantomConfiguration.fanSpeed;
    humidityLevel = phantomConfiguration.humidityLevel;
    ventilationMode = phantomConfiguration.ventilationMode;
    IrSender.begin(4, true);
    IrSender.enableIROut(38);
    xTaskCreate(asyncCommandProcessor, "Async Command Processor", 2048, NULL, 2,
                NULL);
}

void PhantomController::sendCommand(int command) {
    commandsQueue.push(command);
}

bool PhantomController::changeFanSpeed(FanSpeed fanSpeed) {
    int commandCode = fanSpeedCommandCodes[fanSpeed];
    if (commandCode != 0) {
        DeviceConfiguration::savePhantomFanSpeed(fanSpeed);
        PhantomController::fanSpeed = fanSpeed;
        sendCommand(commandCode);
        return true;
    }
    return false;
}

bool PhantomController::changeHumidityLevel(HumidityLevel humidityLevel) {
    int commandCode = humidityLevelCommandCodes[humidityLevel];
    if (commandCode != 0) {
        DeviceConfiguration::savePhantomHumidityLevel(humidityLevel);
        PhantomController::humidityLevel = humidityLevel;
        sendCommand(commandCode);
        return true;
    }
    return false;
}

bool PhantomController::changeVentilationMode(VentilationMode ventilationMode) {
    int commandCode = ventilationModeCommandCodes[ventilationMode];
    if (commandCode != 0) {
        if (ventilationMode == VentilationMode::mode_temporaryEvacuation) {
            setupTempEvac();
        }
        DeviceConfiguration::savePhantomVentilationMode(ventilationMode);
        PhantomController::ventilationMode = ventilationMode;
        sendCommand(commandCode);
        return true;
    }
    return false;
}

bool PhantomController::resetFilter() {
    sendCommand(filterReset);
    return true;
}