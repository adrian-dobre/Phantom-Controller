#include <LivoloRemote.h>

class LivoloController {
   private:
    static LivoloRemote livoloRemote;

   public:
    static void init();
    static void send(unsigned short int remoteId, byte keyCode);
};