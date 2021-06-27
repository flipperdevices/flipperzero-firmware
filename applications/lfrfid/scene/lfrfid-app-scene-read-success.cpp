#include "lfrfid-app-scene-read-success.h"
#include "../view/elements/string-element.h"

void LfRfidAppSceneReadSuccess::on_enter(LfRfidApp* app, bool need_restore) {
    notification_message_block(app->notification, &sequence_set_green_255);
}

bool LfRfidAppSceneReadSuccess::on_event(LfRfidApp* app, LfRfidApp::Event* event) {
    bool consumed = false;

    return consumed;
}

void LfRfidAppSceneReadSuccess::on_exit(LfRfidApp* app) {
    notification_message_block(app->notification, &sequence_reset_green);
}
