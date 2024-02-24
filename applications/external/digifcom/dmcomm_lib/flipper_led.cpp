#include "flipper_led.h"

namespace DMComm {

FlipperIndicator::FlipperIndicator(App* app)
    : app(app) {
}

void FlipperIndicator::begin() {
}

void FlipperIndicator::end() {
}

void FlipperIndicator::new_digirom(BaseDigiROM* digirom) {
    UNUSED(digirom);
    for(uint8_t i = 0; i < 3; i++) {
        notification_message(app->notification, &sequence_set_only_blue_255);
        delay(50);
        notification_message(app->notification, &sequence_reset_rgb);
        delay(50);
    }
}

void FlipperIndicator::new_digirom_error() {
}

void FlipperIndicator::executed_digirom(BaseDigiROM* digirom) {
    notification_message(app->notification, &sequence_set_only_blue_255);
    if(digirom->somethingReceived()) {
        //TODO errors too?
        delay(200);
    } else {
        delay(50);
    }
    notification_message(app->notification, &sequence_reset_rgb);
}

}