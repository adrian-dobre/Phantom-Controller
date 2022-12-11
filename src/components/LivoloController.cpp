#include "LivoloController.h"

#include <queue>

LivoloRemote LivoloController::livoloRemote(26);
struct livoloCommand {
    unsigned short int remoteId;
    byte keyCode;
};

std::queue<livoloCommand> livoloCommandsQueue = std::queue<livoloCommand>();

void livoloAsyncCommandProcessor(void *livoloRemote) {
    while (true) {
        if (!livoloCommandsQueue.empty()) {
            livoloCommand commandToSend = livoloCommandsQueue.front();
            livoloCommandsQueue.pop();
            ((LivoloRemote *)livoloRemote)
                ->send(commandToSend.remoteId, commandToSend.keyCode);
        }
        vTaskDelay(500);
    }
}

void LivoloController::init() {
    xTaskCreate(livoloAsyncCommandProcessor, "Livolo Async Command Processor",
                2048, &livoloRemote, 2, NULL);
}

void LivoloController::send(unsigned short int remoteId, byte keyCode) {
    livoloCommand command = {.remoteId = remoteId, .keyCode = keyCode};
    livoloCommandsQueue.push(command);
}