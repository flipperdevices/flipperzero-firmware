#pragma once
#include "../rfid_debug_app.h"

class RfidDebugAppSceneLFTune : public GenericScene<RfidDebugApp> {
public:
    void on_enter(RfidDebugApp* app, bool need_restore) final;
    bool on_event(RfidDebugApp* app, RfidDebugApp::Event* event) final;
    void on_exit(RfidDebugApp* app) final;
};
