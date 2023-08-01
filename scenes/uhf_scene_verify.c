#include "../uhf_app_i.h"

bool verify_success = false;

void uhf_scene_verify_callback_event(UHFWorkerEvent event, void* ctx) {
    UNUSED(ctx);
    UHFApp* uhf_app = ctx;
    if(event == UHFWorkerEventSuccess) {
        verify_success = true;
        // FURI_LOG_E("verify", "UHFWorkerEventSuccess");
        // FURI_LOG_E("verify", "%d", verify_success);
    } else {
        // FURI_LOG_E("verify", "UHFWorkerEventFail");
    }
    view_dispatcher_send_custom_event(uhf_app->view_dispatcher, UHFCustomEventVerifyDone);
}

void uhf_scene_verify_widget_callback(GuiButtonType result, InputType type, void* ctx) {
    furi_assert(ctx);
    UHFApp* uhf_app = ctx;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(uhf_app->view_dispatcher, result);
    }
}

void uhf_scene_verify_on_enter(void* ctx) {
    UHFApp* uhf_app = ctx;
    uhf_worker_start(
        uhf_app->worker, UHFWorkerStateVerify, uhf_scene_verify_callback_event, uhf_app);
}

bool uhf_scene_verify_on_event(void* ctx, SceneManagerEvent event) {
    UHFApp* uhf_app = ctx;
    bool consumed = false;
    if(event.event == SceneManagerEventTypeBack) {
        uhf_app->worker->state = UHFWorkerStateStop;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(uhf_app->scene_manager, UHFSceneStart);
            consumed = true;
        } else if(event.event == UHFCustomEventVerifyDone) {
            if(verify_success) {
                // FuriString* temp_str = furi_string_alloc();
                // UHFResponseData* response_data = uhf_app->worker->data;
                // UHFData* software_version = uhf_response_data_get_uhf_data(response_data, 0);

                widget_add_string_element(
                    uhf_app->widget, 64, 5, AlignCenter, AlignCenter, FontPrimary, "Module Info");
                // furi_string_cat_str(temp_str, "Software Version: ");
                // for(int i = 0; i < 10; i++) {
                //     furi_string_cat_printf(temp_str, "%c ", software_version->data[6 + i]);
                // }
                // widget_add_string_element(
                //     uhf_app->widget,
                //     3,
                //     10,
                //     AlignLeft,
                //     AlignBottom,
                //     FontSecondary,
                //     furi_string_get_cstr(temp_str));
                widget_add_string_element(
                    uhf_app->widget, 64, 5, AlignCenter, AlignCenter, FontPrimary, "Module Info");
                widget_add_button_element(
                    uhf_app->widget,
                    GuiButtonTypeRight,
                    "Continue",
                    uhf_scene_verify_widget_callback,
                    uhf_app);
                // furi_string_free(temp_str);
            } else {
                widget_add_string_element(
                    uhf_app->widget,
                    64,
                    5,
                    AlignCenter,
                    AlignCenter,
                    FontPrimary,
                    "No UHF Module found");
            }
        }
    }
    return consumed;
}

void uhf_scene_verify_on_exit(void* ctx) {
    UHFApp* uhf_app = ctx;
    // // Stop worker
    uhf_worker_stop(uhf_app->worker);
    // // Clear view
    // popup_reset(uhf_app->popup);
    // clear widget
    widget_reset(uhf_app->widget);
}