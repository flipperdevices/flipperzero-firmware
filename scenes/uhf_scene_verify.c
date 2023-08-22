#include "../uhf_app_i.h"

bool verify_success = false;
FuriString* temp_str;

void uhf_scene_verify_callback_event(UHFWorkerEvent event, void* ctx) {
    UNUSED(ctx);
    UHFApp* uhf_app = ctx;
    if(event == UHFWorkerEventSuccess) verify_success = true;

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
    temp_str = furi_string_alloc();
    view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewWidget);
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
        } else if(event.event == GuiButtonTypeLeft) {
            if(!verify_success) {
                widget_reset(uhf_app->widget);
                furi_string_reset(temp_str);
                uhf_worker_stop(uhf_app->worker);
                // furi_hal_gpio_write(&gpio_ext, false);
                // furi_delay_ms(50);
                // furi_hal_gpio_write(&gpio_ext_pa7, true);
                // furi_delay_ms(50);
                uhf_worker_start(
                    uhf_app->worker,
                    UHFWorkerStateVerify,
                    uhf_scene_verify_callback_event,
                    uhf_app);
            }
        } else if(event.event == UHFCustomEventVerifyDone) {
            if(verify_success) {
                widget_reset(uhf_app->widget);
                furi_string_reset(temp_str);
                M100Module* module = uhf_app->worker->module;
                widget_add_string_element(
                    uhf_app->widget, 64, 5, AlignCenter, AlignCenter, FontPrimary, "Module Info");
                // hardware info
                furi_string_cat_str(temp_str, "HW Version: ");
                furi_string_cat_str(temp_str, module->info->hw_version);
                widget_add_string_element(
                    uhf_app->widget,
                    1,
                    15,
                    AlignLeft,
                    AlignTop,
                    FontSecondary,
                    furi_string_get_cstr(temp_str));
                furi_string_reset(temp_str);
                // software info
                furi_string_cat_str(temp_str, "SW Version: ");
                furi_string_cat_str(temp_str, module->info->sw_version);
                widget_add_string_element(
                    uhf_app->widget,
                    1,
                    27,
                    AlignLeft,
                    AlignTop,
                    FontSecondary,
                    furi_string_get_cstr(temp_str));
                furi_string_reset(temp_str);
                // manufacturer info
                furi_string_cat_str(temp_str, "Manufacturer: ");
                furi_string_cat_str(temp_str, module->info->manufacturer);
                widget_add_string_element(
                    uhf_app->widget,
                    1,
                    39,
                    AlignLeft,
                    AlignTop,
                    FontSecondary,
                    furi_string_get_cstr(temp_str));

                widget_add_button_element(
                    uhf_app->widget,
                    GuiButtonTypeRight,
                    "Continue",
                    uhf_scene_verify_widget_callback,
                    uhf_app);
            } else {
                widget_add_string_element(
                    uhf_app->widget,
                    64,
                    5,
                    AlignCenter,
                    AlignCenter,
                    FontPrimary,
                    "No UHF Module found");
                widget_add_string_multiline_element(
                    uhf_app->widget,
                    64,
                    30,
                    AlignCenter,
                    AlignCenter,
                    FontSecondary,
                    "Please refer to the git@frux-c/uhf_rfid for help.");
                widget_add_button_element(
                    uhf_app->widget,
                    GuiButtonTypeLeft,
                    "Retry",
                    uhf_scene_verify_widget_callback,
                    uhf_app);
                widget_add_button_element(
                    uhf_app->widget,
                    GuiButtonTypeRight,
                    "Skip",
                    uhf_scene_verify_widget_callback,
                    uhf_app);
            }
        }
    }
    return consumed;
}

void uhf_scene_verify_on_exit(void* ctx) {
    UHFApp* uhf_app = ctx;
    // Clear string
    furi_string_free(temp_str);
    // Stop worker
    uhf_worker_stop(uhf_app->worker);
    // clear widget
    widget_reset(uhf_app->widget);
}