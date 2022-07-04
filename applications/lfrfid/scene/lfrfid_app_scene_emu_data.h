#pragma once
#include "../lfrfid_app.h"

class LfRfidAppSceneEmuData : public GenericScene<LfRfidApp> {
public:
    void on_enter(LfRfidApp* app, bool /*need_restore*/) final;
    bool on_event(LfRfidApp* app, LfRfidApp::Event* event) final;
    void on_exit(LfRfidApp* app) final;

private:
    static void save_callback(void* context);

    uint8_t key_data[LFRFID_KEY_SIZE] = {
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
    };
};
