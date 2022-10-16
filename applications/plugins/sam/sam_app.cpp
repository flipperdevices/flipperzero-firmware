#include <furi.h>
#include "stm32_sam.h"

STM32SAM voice;

extern "C" int32_t sam_app(void* p) {
    UNUSED(p);

    voice.begin();
    voice.say("Hello world. I am FLIPPER SAM, automated voice!");

    return 0;
}