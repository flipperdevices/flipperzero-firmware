/*

Wish list:

1.  variable_item_set_current_value_text allows for large text, but
it trucates it to X characters.  It would be nice it it scrolled.


*/

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include "rolling_flaws_icons.h"

#include <lib/subghz/receiver.h>
#include <lib/subghz/protocols/protocol_items.h>
#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>
#include <lib/subghz/devices/devices.h>

#include "rolling_flaws_subghz_receive.h"
#include "rolling_flaws_settings.h"
#include "rolling_flaws_about.h"
#include "rolling_flaws_keeloq.h"
#include "rolling_flaws_send_keeloq.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "RollingFlawsSubGHzApp"

// Comment this line if you don't want the backlight to be continuously on.
#define BACKLIGHT_ALWAYS_ON yes

typedef enum {
    RollingFlawsSubmenuIndexConfigure,
    RollingFlawsSubmenuIndexResetCountToZero,
    RollingFlawsSubmenuIndexTransmit,
    RollingFlawsSubmenuIndexReceive,
    RollingFlawsSubmenuIndexSyncRemote,
    RollingFlawsSubmenuIndexAbout,
} RollingFlawsSubmenuIndex;

typedef enum {
    RollingFlawsViewSubmenu,
    RollingFlawsViewConfigure,
    RollingFlawsViewReceiveSignals,
    RollingFlawsViewReceiveSync,
    RollingFlawsViewAbout,
} RollingFlawsView;

typedef enum {
    RollingFlawsEventIdReceivedSignal,
} RollingFlawsEventId;

static bool decode_packet(FuriString* buffer, void* ctx) {
    RollingFlaws* context = ctx;
    if(furi_string_start_with_str(buffer, "KeeLoq 64bit")) {
        if(!furi_string_start_with_str(
               buffer, rolling_flaws_setting_protocol_base_name_get(context->model))) {
            FURI_LOG_I(TAG, "KeeLoq 64bit protocol is not enabled");
            return true;
        }
        decode_keeloq(context->model, buffer, false);
    } else {
        FURI_LOG_I(TAG, "Unknown protocol");
    }

    return true;
}

static bool sync_packet(FuriString* buffer, void* ctx) {
    RollingFlaws* context = ctx;
    if(furi_string_start_with_str(buffer, "KeeLoq 64bit")) {
        if(!furi_string_start_with_str(
               buffer, rolling_flaws_setting_protocol_base_name_get(context->model))) {
            FURI_LOG_I(TAG, "KeeLoq 64bit protocol is not enabled");
            return true;
        }
        decode_keeloq(context->model, buffer, true);
        view_dispatcher_send_custom_event(
            context->view_dispatcher, RollingFlawsEventIdReceivedSignal);
        return false;
    } else {
        FURI_LOG_I(TAG, "Unknown protocol");
    }

    return true;
}

/**
 * @brief      Callback for navigation events
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      context  The context
 * @return     next view id
*/
uint32_t rolling_flaws_navigation_exit_callback(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

/**
 * @brief      Callback for navigation events
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      context  The context
 * @return     next view id
*/
uint32_t rolling_flaws_navigation_submenu_callback(void* context) {
    UNUSED(context);

    return RollingFlawsViewSubmenu;
}

/**
 * @brief      Callback for navigation events
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      context  The context
 * @return     next view id
*/
uint32_t rolling_flaws_navigation_submenu_stop_receiving_callback(void* context) {
    RollingFlaws* app = (RollingFlaws*)context;
    stop_listening(app->subghz);

    return RollingFlawsViewSubmenu;
}

uint32_t rolling_flaws_navigation_submenu_stop_sync_callback(void* context) {
    RollingFlaws* app = (RollingFlaws*)context;
    stop_listening(app->subghz);

    return RollingFlawsViewSubmenu;
}

bool rolling_flaws_view_dispatcher_custom_event_callback(void* context, uint32_t event) {
    FURI_LOG_I(TAG, "Custom event received: %ld", event);
    if(event == RollingFlawsEventIdReceivedSignal) {
        RollingFlaws* app = (RollingFlaws*)context;
        stop_listening(app->subghz);

        furi_hal_vibro_on(true);
        furi_delay_ms(200);
        furi_hal_vibro_on(false);
        furi_delay_ms(100);

        furi_hal_vibro_on(true);
        furi_delay_ms(100);
        furi_hal_vibro_on(false);

        view_dispatcher_switch_to_view(app->view_dispatcher, RollingFlawsViewSubmenu);
        return true;
    }

    return false;
}

void rolling_flaws_submenu_callback(void* context, uint32_t index) {
    RollingFlaws* app = (RollingFlaws*)context;

    switch(index) {
    case RollingFlawsSubmenuIndexConfigure:
        view_dispatcher_switch_to_view(app->view_dispatcher, RollingFlawsViewConfigure);
        break;
    case RollingFlawsSubmenuIndexResetCountToZero:
        app->model->count = 0x0;
        app->model->future_count = 0xFFFFFFFF;
        furi_hal_vibro_on(true);
        furi_delay_ms(200);
        furi_hal_vibro_on(false);
        break;
    case RollingFlawsSubmenuIndexTransmit:
        app->model->count++;
        app->model->future_count = 0xFFFFFFFF;
        send_keeloq_count(
            rolling_flaws_setting_fix_get(app->model),
            app->model->count - 2,
            rolling_flaws_setting_protocol_mf_name_get(app->model),
            rolling_flaws_setting_frequency_get(app->model));
        furi_hal_vibro_on(true);
        furi_delay_ms(100);
        furi_hal_vibro_on(false);
        furi_delay_ms(100);
        furi_hal_vibro_on(true);
        furi_delay_ms(200);
        furi_hal_vibro_on(false);
        break;
    case RollingFlawsSubmenuIndexReceive: {
        uint32_t frequency = rolling_flaws_setting_frequency_get(app->model);
        app->model->opened = false;
        start_listening(app->subghz, frequency, decode_packet, app);
        view_dispatcher_switch_to_view(app->view_dispatcher, RollingFlawsViewReceiveSignals);
    } break;
    case RollingFlawsSubmenuIndexSyncRemote: {
        uint32_t frequency = rolling_flaws_setting_frequency_get(app->model);
        start_listening(app->subghz, frequency, sync_packet, app);
        view_dispatcher_switch_to_view(app->view_dispatcher, RollingFlawsViewReceiveSync);
    } break;
    case RollingFlawsSubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, RollingFlawsViewAbout);
        break;
    default:
        break;
    }
}

void rolling_flaws_receive_sync_draw_callback(Canvas* canvas, void* model) {
    UNUSED(model);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 13, 30, "Syncing rolling code:");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 13, 45, "Press remote button now.");
}

void rolling_flaws_receive_signal_draw_callback(Canvas* canvas, void* model) {
    RollingFlawsModel* my_model = ((RollingFlawsRefModel*)model)->model;

    FuriString* str = furi_string_alloc(32);

    canvas_set_bitmap_mode(canvas, 1);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 13, 8, "Rolling code receiver");
    canvas_set_font(canvas, FontSecondary);
    furi_string_printf(str, "Count:   %04X", (uint16_t)my_model->count);
    canvas_draw_str(canvas, 2, 34, furi_string_get_cstr(str));
    canvas_set_font(canvas, FontSecondary);
    if(my_model->future_count > 0xFFFF) {
        canvas_draw_str(canvas, 2, 44, "Future:   none");
    } else {
        furi_string_printf(str, "Future:   %04X", (uint16_t)my_model->future_count);
        canvas_draw_str(canvas, 2, 44, furi_string_get_cstr(str));
    }
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 3, 20, rolling_flaws_setting_protocol_display_name_get(my_model));
    canvas_set_font(canvas, FontSecondary);
    furi_string_printf(str, "Fix: %08lX", rolling_flaws_setting_fix_get(my_model));
    canvas_draw_str(canvas, 2, 54, furi_string_get_cstr(str));
    canvas_set_font(canvas, FontSecondary);
    furi_string_printf(str, "RX: %s", furi_string_get_cstr(my_model->key));
    canvas_draw_str(canvas, 2, 64, furi_string_get_cstr(str));
    if(my_model->opened) {
        canvas_draw_icon(canvas, 100, 15, &I_Unlock_10x8);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 82, 33, "OPENED!");
    } else {
        canvas_draw_icon(canvas, 100, 15, &I_Lock_10x8);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 85, 33, "CLOSED");
    }

    canvas_set_font(canvas, FontSecondary);
    furi_string_printf(str, "%sMHz", rolling_flaws_setting_frequency_name_get(my_model));
    canvas_draw_str(canvas, 75, 43, furi_string_get_cstr(str));

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 82, 54, furi_string_get_cstr(my_model->status));

    furi_string_free(str);
}

bool rolling_flaws_view_input_callback(InputEvent* event, void* context) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

RollingFlaws* rolling_flaws_alloc() {
    RollingFlaws* app = (RollingFlaws*)malloc(sizeof(RollingFlaws));

    Gui* gui = furi_record_open(RECORD_GUI);

    app->subghz = rolling_flaws_subghz_alloc();

    app->model = malloc(sizeof(RollingFlawsModel));
    app->model->key = furi_string_alloc();
    app->model->custom_mf = furi_string_alloc();
    app->model->status = furi_string_alloc();
    app->model->custom_fix = 0x24321234;
    app->model->count = 0x0;
    app->model->future_count = 0xFFFFFFFF;
    app->model->opened = false;

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, rolling_flaws_view_dispatcher_custom_event_callback);

    app->submenu = submenu_alloc();
    submenu_add_item(
        app->submenu,
        "Config",
        RollingFlawsSubmenuIndexConfigure,
        rolling_flaws_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Reset count to 0",
        RollingFlawsSubmenuIndexResetCountToZero,
        rolling_flaws_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Transmit Signal",
        RollingFlawsSubmenuIndexTransmit,
        rolling_flaws_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Receive Signals",
        RollingFlawsSubmenuIndexReceive,
        rolling_flaws_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Sync Remote",
        RollingFlawsSubmenuIndexSyncRemote,
        rolling_flaws_submenu_callback,
        app);
    submenu_add_item(
        app->submenu, "About", RollingFlawsSubmenuIndexAbout, rolling_flaws_submenu_callback, app);
    view_set_previous_callback(
        submenu_get_view(app->submenu), rolling_flaws_navigation_exit_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, RollingFlawsViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, RollingFlawsViewSubmenu);

    rolling_flaw_populate_variable_item_list(app);
    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_config),
        rolling_flaws_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher,
        RollingFlawsViewConfigure,
        variable_item_list_get_view(app->variable_item_list_config));

    app->view_receive_signals = view_alloc();
    view_set_context(app->view_receive_signals, app);
    view_set_draw_callback(app->view_receive_signals, rolling_flaws_receive_signal_draw_callback);
    view_set_input_callback(app->view_receive_signals, rolling_flaws_view_input_callback);
    view_set_previous_callback(
        app->view_receive_signals, rolling_flaws_navigation_submenu_stop_receiving_callback);
    view_allocate_model(
        app->view_receive_signals, ViewModelTypeLockFree, sizeof(RollingFlawsRefModel));
    RollingFlawsRefModel* refmodel = view_get_model(app->view_receive_signals);
    refmodel->model = app->model;
    view_dispatcher_add_view(
        app->view_dispatcher, RollingFlawsViewReceiveSignals, app->view_receive_signals);

    app->view_receive_sync = view_alloc();
    view_set_context(app->view_receive_sync, app);
    view_set_draw_callback(app->view_receive_sync, rolling_flaws_receive_sync_draw_callback);
    view_set_input_callback(app->view_receive_sync, rolling_flaws_view_input_callback);
    view_set_previous_callback(
        app->view_receive_sync, rolling_flaws_navigation_submenu_stop_sync_callback);
    view_allocate_model(
        app->view_receive_sync, ViewModelTypeLockFree, sizeof(RollingFlawsRefModel));
    refmodel = view_get_model(app->view_receive_sync);
    refmodel->model = app->model;
    view_dispatcher_add_view(
        app->view_dispatcher, RollingFlawsViewReceiveSync, app->view_receive_sync);

    app->widget_about = widget_alloc();
    widget_add_text_scroll_element(app->widget_about, 0, 0, 128, 64, ROLLING_FLAWS_ABOUT_TEXT);
    view_set_previous_callback(
        widget_get_view(app->widget_about), rolling_flaws_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, RollingFlawsViewAbout, widget_get_view(app->widget_about));

    app->notifications = furi_record_open(RECORD_NOTIFICATION);

#ifdef BACKLIGHT_ALWAYS_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_on);
#endif

    return app;
}

void rolling_flaws_free(RollingFlaws* app) {
#ifdef BACKLIGHT_ALWAYS_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_auto);
#endif
    furi_record_close(RECORD_NOTIFICATION);

    view_dispatcher_remove_view(app->view_dispatcher, RollingFlawsViewAbout);
    widget_free(app->widget_about);
    view_dispatcher_remove_view(app->view_dispatcher, RollingFlawsViewReceiveSignals);
    view_free(app->view_receive_signals);
    view_dispatcher_remove_view(app->view_dispatcher, RollingFlawsViewReceiveSync);
    view_free(app->view_receive_sync);
    view_dispatcher_remove_view(app->view_dispatcher, RollingFlawsViewConfigure);
    variable_item_list_free(app->variable_item_list_config);
    view_dispatcher_remove_view(app->view_dispatcher, RollingFlawsViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    rolling_flaws_subghz_free(app->subghz);

    free(app);
}

int32_t rolling_flaws_app(void* p) {
    UNUSED(p);

    RollingFlaws* app = rolling_flaws_alloc();
    view_dispatcher_run(app->view_dispatcher);

    rolling_flaws_free(app);
    return 0;
}