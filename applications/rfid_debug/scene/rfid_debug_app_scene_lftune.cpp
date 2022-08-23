#include "rfid_debug_app_scene_lftune.h"
#include <furi_hal.h>

static void comparator_trigger_callback(bool level, void* comp_ctx) {
    UNUSED(comp_ctx);
    furi_hal_gpio_write(&gpio_ext_pa7, !level);
}

void RfidDebugAppSceneLFTune::on_enter(RfidDebugApp* app, bool /* need_restore */) {
    app->view_controller.switch_to<RfidViewLFTuneVM>();
    furi_hal_gpio_init_simple(&gpio_ext_pa7, GpioModeOutputPushPull);

    furi_hal_rfid_comp_set_callback(comparator_trigger_callback, this);
    furi_hal_rfid_comp_start();

    furi_hal_rfid_pins_read();
    furi_hal_rfid_tim_read(125000, 0.5);
    furi_hal_rfid_tim_read_start();
}

bool RfidDebugAppSceneLFTune::on_event(RfidDebugApp* app, RfidDebugApp::Event* /* event */) {
    bool consumed = false;

    RfidViewLFTuneVM* LFtune = app->view_controller;

    if(LFtune->is_dirty()) {
        furi_hal_rfid_set_read_period(LFtune->get_ARR());
        furi_hal_rfid_set_read_pulse(LFtune->get_CCR());
    }

    return consumed;
}

void RfidDebugAppSceneLFTune::on_exit(RfidDebugApp* /* app */) {
    furi_hal_rfid_comp_stop();
    furi_hal_rfid_comp_set_callback(NULL, NULL);

    furi_hal_gpio_init_simple(&gpio_ext_pa7, GpioModeAnalog);
    furi_hal_rfid_tim_read_stop();
    furi_hal_rfid_tim_reset();
    furi_hal_rfid_pins_reset();
}
