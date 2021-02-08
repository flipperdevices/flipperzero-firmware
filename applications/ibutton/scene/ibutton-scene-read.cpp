#include "ibutton-scene-start.h"
#include "../ibutton-app.h"
#include "../ibutton-view-manager.h"
#include "../ibutton-event.h"

typedef enum {
    SubmenuIndexRead,
    SubmenuIndexSaved,
    SubmenuIndexAdd,
} SubmenuIndex;

void iButtonSceneRead::on_enter(iButtonApp* app) {
    iButtonAppViewManager* view_manager = app->get_view_manager();
    Popup* popup = view_manager->get_popup();

    popup_set_header(popup, "iButton", 95, 26, AlignCenter, AlignBottom);
    popup_set_text(popup, "waiting\nfor key ...", 95, 32, AlignCenter, AlignTop);
    popup_set_icon(popup, 0, 5, I_DolphinWait_61x59);

    view_manager->switch_to(iButtonAppViewManager::Type::iButtonAppViewPopup);
    app->get_onewire_master()->start();
}

bool iButtonSceneRead::on_event(iButtonApp* app, iButtonEvent* event) {
    bool consumed = false;

    if(event->type == iButtonEvent::Type::EventTypeTick) {
        bool result = 0;
        const uint8_t key_size = app->get_key()->get_size();
        uint8_t address[key_size];
        uint8_t address_second[key_size];
        OneWireMaster* onewire = app->get_onewire_master();

        consumed = true;
        app->notify_red_blink();

        osKernelLock();
        result = onewire->reset();
        osKernelUnlock();

        if(result) {
            osKernelLock();
            __disable_irq();
            onewire->write(0x33);
            onewire->read_bytes(address, key_size);
            __enable_irq();
            osKernelUnlock();

            if(maxim_crc8(address, key_size) == 0) {
                if(address[0] == 0x01) {
                    app->notify_success();
                } else {
                    // not are key error
                    app->get_key()->set_data(address, key_size);
                    app->switch_to_next_scene(iButtonApp::Scene::SceneReadNotKeyError);
                }
            } else {
                // read twice, if keys are same - we get crc error
                delay(100);

                osKernelLock();
                result = onewire->reset();
                osKernelUnlock();

                if(result) {
                    osKernelLock();
                    __disable_irq();
                    onewire->write(0x33);
                    onewire->read_bytes(address_second, key_size);
                    __enable_irq();
                    osKernelUnlock();

                    if(memcmp(address, address_second, key_size) == 0) {
                        // crc error
                        app->get_key()->set_data(address, key_size);
                        app->switch_to_next_scene(iButtonApp::Scene::SceneReadCRCError);
                    }
                }
            }
        }
    }

    return consumed;
}

void iButtonSceneRead::on_exit(iButtonApp* app) {
    Popup* popup = app->get_view_manager()->get_popup();

    popup_set_header(popup, NULL, 0, 0, AlignCenter, AlignBottom);
    popup_set_text(popup, NULL, 0, 0, AlignCenter, AlignTop);
    popup_set_icon(popup, -1, -1, I_DolphinWait_61x59);

    app->get_onewire_master()->stop();
}