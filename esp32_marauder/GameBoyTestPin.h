#ifndef GameBoyTestPin_h
#define GameBoyTestPin_h

#include "Gameboy.h"

class GameBoyTestPin {

  public:
    GameBoyTestPin();
    void begin();
    void main();
    void setup();
    void highPin(int pin);

};

#endif