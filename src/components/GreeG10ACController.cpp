#include "GreeG10ACController.h"

#include <GreeHeatpumpIR.h>

#include <queue>

IRSenderESP32 irSender(4, 0);
GreeHeatpumpIR *acIR = new GreeYANHeatpumpIR();
std::queue<ACCommand> greeCommandsQueue = std::queue<ACCommand>();

void greeAsyncCommandProcessor(void *arg) {
    while (true) {
        if (!greeCommandsQueue.empty()) {
            ACCommand commandToSend = greeCommandsQueue.front();
            greeCommandsQueue.pop();
            // increase the likelyhood the HVAC receives the command
            int repeatSend = 2;
            while (repeatSend > 0) {
                repeatSend--;
                acIR->send(irSender, commandToSend.power, commandToSend.mode,
                           commandToSend.fanSpeed, commandToSend.temperature,
                           VDIR_AUTO, HDIR_AUTO, commandToSend.turbo);
                vTaskDelay(50);
            }
        }
        vTaskDelay(500);
    }
}

void GreeG10ACController::init() {
    xTaskCreate(greeAsyncCommandProcessor, "Async Command Processor", 2048, NULL, 2,
                NULL);
}

void GreeG10ACController::send(int power, int mode, int fanSpeed,
                               int temperature, bool turbo) {
    ACCommand command = {
        .power = power,
        .mode = mode,
        .fanSpeed = fanSpeed,
        .temperature = temperature,
        .turbo = turbo
    };
    greeCommandsQueue.push(command);
}
