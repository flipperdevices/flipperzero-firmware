#pragma once
#include "../rfid_debug_app.h"

class RfidDebugAppSceneStart : public GenericScene<RfidDebugApp> {
public:
    void on_enter(RfidDebugApp* app, bool need_restore) final;
    bool on_event(RfidDebugApp* app, RfidDebugApp::Event* event) final;
    void on_exit(RfidDebugApp* app) final;

private:
    void draw_menu(RfidDebugApp* app, int32_t selected);
    void submenu_callback(void* context, uint32_t index);
    uint32_t submenu_item_selected = 0;
};
