#include "ibutton-scene-start.h"
#include "../ibutton-app.h"
#include "../ibutton-view.h"
#include "../ibutton-event.h"

typedef enum {
    SubmenuIndexRead,
    SubmenuIndexSaved,
    SubmenuIndexAdd,
} SubmenuIndex;

void iButtonSceneRead::on_enter(iButtonApp* app) {
    iButtonAppView* view = app->get_view();
    Popup* popup = view->get_popup();

    popup_set_header(popup, "iButton", 95, 26, AlignCenter, AlignBottom);
    popup_set_text(popup, "waiting\nfor key ...", 95, 32, AlignCenter, AlignTop);
    popup_set_icon(popup, 0, 5, I_DolphinWait_61x59);

    view->switch_to(iButtonAppView::Type::iButtonAppViewPopup);
    app->get_onewire_master()->start();
}

bool iButtonSceneRead::on_event(iButtonApp* app, iButtonEvent* event) {
    bool consumed = false;

    if(event->type == iButtonEvent::Type::EventTypeTick) {
        consumed = true;
        app->notify_red_blink();

        bool result = 0;
        uint8_t address[8];
        OneWireMaster* onewire = app->get_onewire_master();

        osKernelLock();
        result = onewire->reset();
        osKernelUnlock();

        if(result) {
            osKernelLock();
            __disable_irq();
            onewire->write(0x33);
            onewire->read_bytes(address, 8);
            __enable_irq();
            osKernelUnlock();

            if(maxim_crc8(address, 8) == 0) {
                app->notify_green_on();
            }
        }
    }

    return consumed;
}

void iButtonSceneRead::on_exit(iButtonApp* app) {
    Popup* popup = app->get_view()->get_popup();

    popup_set_header(popup, NULL, 0, 0, AlignCenter, AlignBottom);
    popup_set_text(popup, NULL, 0, 0, AlignCenter, AlignTop);
    popup_set_icon(popup, -1, -1, I_DolphinWait_61x59);

    app->get_onewire_master()->stop();
}