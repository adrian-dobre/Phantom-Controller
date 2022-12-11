struct ACCommand {
    int power;
    int mode;
    int fanSpeed;
    int temperature;
    bool turbo;
};

class GreeG10ACController {
   public:
    static void init();
    static void send(int power, int mode, int fanSpeed, int temperature,
                     bool turbo);
};