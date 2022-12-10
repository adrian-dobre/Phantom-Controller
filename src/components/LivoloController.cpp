#include "LivoloController.h"

#include <queue>

LivoloRemote LivoloController::livoloRemote(26);

void LivoloController::send(unsigned short int remoteId, byte keyCode) {
   LivoloController::livoloRemote.send(remoteId, keyCode);
}