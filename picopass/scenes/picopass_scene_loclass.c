#include "../picopass_i.h"
#include <dolphin/dolphin.h>

static NfcCommand
    picopass_scene_loclass_listener_callback(PicopassListenerEvent event, void* context) {
    NfcCommand command = NfcCommandContinue;

    Picopass* picopass = context;

    if(event.type == PicopassListenerEventTypeLoclassGotMac) {
        picopass->loclass_context.macs_collected++;
        view_dispatcher_send_custom_event(
            picopass->view_dispatcher, PicopassCustomEventLoclassGotMac);
    } else if(event.type == PicopassListenerEventTypeLoclassGotStandardKey) {
        view_dispatcher_send_custom_event(
            picopass->view_dispatcher, PicopassCustomEventLoclassGotStandardKey);
        command = NfcCommandStop;
    }

    return command;
}

static void picopass_loclass_result_callback(void* context) {
    furi_assert(context);

    Picopass* picopass = context;
    view_dispatcher_send_custom_event(picopass->view_dispatcher, PicopassCustomEventViewExit);
}

void picopass_scene_loclass_on_enter(void* context) {
    Picopass* picopass = context;

    dolphin_deed(DolphinDeedNfcEmulate);

    scene_manager_set_scene_state(picopass->scene_manager, PicopassSceneLoclass, 0);

    loclass_set_callback(picopass->loclass, picopass_loclass_result_callback, picopass);
    loclass_set_header(picopass->loclass, "Loclass");

    picopass_blink_emulate_start(picopass);
    view_dispatcher_switch_to_view(picopass->view_dispatcher, PicopassViewLoclass);

    PicopassDeviceData* data = malloc(sizeof(PicopassDeviceData));
    const uint8_t config_block[PICOPASS_BLOCK_LEN] = {
        0x12, 0xFF, 0xFF, 0xFF, 0x7F, 0x1F, 0xFF, 0x3C};
    memcpy(data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data, config_block, sizeof(config_block));

    const uint8_t epurse[PICOPASS_BLOCK_LEN] = {0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    memcpy(data->AA1[PICOPASS_SECURE_EPURSE_BLOCK_INDEX].data, epurse, sizeof(epurse));

    const uint8_t aia[PICOPASS_BLOCK_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    memcpy(data->AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data, aia, sizeof(aia));

    picopass->listener = picopass_listener_alloc(picopass->nfc, data);
    free(data);
    if(picopass_listener_set_mode(picopass->listener, PicopassListenerModeLoclass)) {
        picopass_listener_start(
            picopass->listener, picopass_scene_loclass_listener_callback, picopass);
    } else {
        loclass_set_num_macs(picopass->loclass, 255);
        loclass_set_header(picopass->loclass, "Error Opening Log File");
        picopass_listener_free(picopass->listener);
        picopass->listener = NULL;
    }
}

bool picopass_scene_loclass_on_event(void* context, SceneManagerEvent event) {
    bool consumed = false;
    Picopass* picopass = context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PicopassCustomEventLoclassGotMac) {
            notification_message(picopass->notifications, &sequence_single_vibro);
            loclass_set_num_macs(picopass->loclass, picopass->loclass_context.macs_collected);
            if(picopass->loclass_context.macs_collected >= LOCLASS_MACS_TO_COLLECT) {
                notification_message(picopass->notifications, &sequence_double_vibro);
                scene_manager_previous_scene(picopass->scene_manager);
            }
            consumed = true;
        } else if(event.event == PicopassCustomEventLoclassGotStandardKey) {
            loclass_set_header(picopass->loclass, "Loclass (Got Std Key)");
            notification_message(picopass->notifications, &sequence_error);
            consumed = true;
        } else if(event.event == PicopassCustomEventViewExit) {
            consumed = scene_manager_previous_scene(picopass->scene_manager);
        }
    }

    return consumed;
}

void picopass_scene_loclass_on_exit(void* context) {
    Picopass* picopass = context;

    if(picopass->listener) {
        picopass_listener_stop(picopass->listener);
        picopass_listener_free(picopass->listener);
    }
    picopass->loclass_context.macs_collected = 0;
    picopass_blink_stop(picopass);

    loclass_reset(picopass->loclass);
}