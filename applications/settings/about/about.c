#include <furi.h>
#include <dialogs/dialogs.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/empty_screen.h>
#include <gui/elements.h>
#include <assets_icons.h>
#include <furi_hal_version.h>
#include <furi_hal_region.h>
#include <furi_hal_bt.h>
#include <power/power_service/power.h>

int screen_index;

#define LOW_CHARGE_THRESHOLD 10
#define HIGH_DRAIN_CURRENT_THRESHOLD 100

typedef DialogMessageButton (*AboutDialogScreen)(DialogsApp* dialogs, DialogMessage* message);

static DialogMessageButton product_screen(DialogsApp* dialogs, DialogMessage* message) {
    DialogMessageButton result;

    FuriString* screen_header = furi_string_alloc_printf(
        "Product: %s\n"
        "Model: %s",
        furi_hal_version_get_model_name(),
        furi_hal_version_get_model_code());

    FuriString* screen_text = furi_string_alloc_printf(
        "FCC ID: %s\n"
        "IC: %s",
        furi_hal_version_get_fcc_id(),
        furi_hal_version_get_ic_id());

    dialog_message_set_header(
        message, furi_string_get_cstr(screen_header), 0, 0, AlignLeft, AlignTop);
    dialog_message_set_text(
        message, furi_string_get_cstr(screen_text), 0, 26, AlignLeft, AlignTop);
    result = dialog_message_show(dialogs, message);
    dialog_message_set_header(message, NULL, 0, 0, AlignLeft, AlignTop);
    dialog_message_set_text(message, NULL, 0, 0, AlignLeft, AlignTop);

    furi_string_free(screen_header);
    furi_string_free(screen_text);

    return result;
}

static DialogMessageButton address_screen(DialogsApp* dialogs, DialogMessage* message) {
    DialogMessageButton result;

    const char* screen_text = "Flipper Devices Inc\n"
                              "Suite B #551, 2803\n"
                              "Philadelphia Pike, Claymont\n"
                              "DE, USA 19703\n";

    dialog_message_set_text(message, screen_text, 0, 0, AlignLeft, AlignTop);
    result = dialog_message_show(dialogs, message);
    dialog_message_set_text(message, NULL, 0, 0, AlignLeft, AlignTop);

    return result;
}

static DialogMessageButton compliance_screen(DialogsApp* dialogs, DialogMessage* message) {
    DialogMessageButton result;

    const char* screen_text = "For all compliance\n"
                              "certificates please visit:\n"
                              "www.flipp.dev/compliance";

    dialog_message_set_text(message, screen_text, 0, 0, AlignLeft, AlignTop);
    result = dialog_message_show(dialogs, message);
    dialog_message_set_text(message, NULL, 0, 0, AlignLeft, AlignTop);

    return result;
}

static DialogMessageButton icon1_screen(DialogsApp* dialogs, DialogMessage* message) {
    DialogMessageButton result;

    dialog_message_set_icon(message, &I_Certification1_103x56, 13, 0);
    result = dialog_message_show(dialogs, message);
    dialog_message_set_icon(message, NULL, 0, 0);

    return result;
}

static DialogMessageButton icon2_screen(DialogsApp* dialogs, DialogMessage* message) {
    DialogMessageButton result;

    dialog_message_set_icon(message, &I_Certification2_98x33, 15, 10);
    result = dialog_message_show(dialogs, message);
    dialog_message_set_icon(message, NULL, 0, 0);

    return result;
}

static DialogMessageButton hw_version_screen(DialogsApp* dialogs, DialogMessage* message) {
    DialogMessageButton result;
    FuriString* buffer;
    buffer = furi_string_alloc();
    const char* my_name = furi_hal_version_get_name_ptr();

    furi_string_cat_printf(
        buffer,
        "%d.F%dB%dC%d %s:%s %s\n",
        furi_hal_version_get_hw_version(),
        furi_hal_version_get_hw_target(),
        furi_hal_version_get_hw_body(),
        furi_hal_version_get_hw_connect(),
        furi_hal_version_get_hw_region_name_otp(),
        furi_hal_region_get_name(),
        my_name ? my_name : "Unknown");

    furi_string_cat_printf(buffer, "Serial Number:\n");
    const uint8_t* uid = furi_hal_version_uid();
    for(size_t i = 0; i < furi_hal_version_uid_size(); i++) {
        furi_string_cat_printf(buffer, "%02X", uid[i]);
    }

    dialog_message_set_header(message, "HW Version Info:", 0, 0, AlignLeft, AlignTop);
    dialog_message_set_text(message, furi_string_get_cstr(buffer), 0, 13, AlignLeft, AlignTop);
    result = dialog_message_show(dialogs, message);
    dialog_message_set_text(message, NULL, 0, 0, AlignLeft, AlignTop);
    dialog_message_set_header(message, NULL, 0, 0, AlignLeft, AlignTop);
    furi_string_free(buffer);

    return result;
}

static DialogMessageButton fw_version_screen(DialogsApp* dialogs, DialogMessage* message) {
    DialogMessageButton result;
    FuriString* buffer;
    buffer = furi_string_alloc();
    const Version* ver = furi_hal_version_get_firmware_version();
    const BleGlueC2Info* c2_ver = NULL;
#ifdef SRV_BT
    c2_ver = ble_glue_get_c2_info();
#endif

    if(!ver) { //-V1051
        furi_string_cat_printf(buffer, "No info\n");
    } else {
        furi_string_cat_printf(
            buffer,
            "%s [%s]\n%s%s [%s] %s\n[%d] %s",
            version_get_version(ver),
            version_get_builddate(ver),
            version_get_dirty_flag(ver) ? "[!] " : "",
            version_get_githash(ver),
            version_get_gitbranchnum(ver),
            c2_ver ? c2_ver->StackTypeString : "<none>",
            version_get_target(ver),
            version_get_gitbranch(ver));
    }

    dialog_message_set_header(message, "FW Version Info:", 0, 0, AlignLeft, AlignTop);
    dialog_message_set_text(message, furi_string_get_cstr(buffer), 0, 13, AlignLeft, AlignTop);
    result = dialog_message_show(dialogs, message);
    dialog_message_set_text(message, NULL, 0, 0, AlignLeft, AlignTop);
    dialog_message_set_header(message, NULL, 0, 0, AlignLeft, AlignTop);
    furi_string_free(buffer);

    return result;
}

static void draw_stat(Canvas* canvas, int x, int y, const Icon* icon, char* val) {
    canvas_draw_frame(canvas, x - 7, y + 7, 30, 13);
    canvas_draw_icon(canvas, x, y, icon);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, x - 4, y + 16, 24, 6);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_str_aligned(canvas, x + 8, y + 22, AlignCenter, AlignBottom, val);
};

static void draw_battery(Canvas* canvas, PowerInfo* info, int x, int y) {
    char header[20] = {};
    char value[20] = {};

    int32_t drain_current = info->current_gauge * (-1000);
    uint32_t charge_current = info->current_gauge * 1000;

    // Draw battery
    canvas_draw_icon(canvas, x, y, &I_BatteryBody_52x28);
    if(charge_current > 0) {
        canvas_draw_icon(canvas, x + 16, y + 7, &I_FaceCharging_29x14);
    } else if(drain_current > HIGH_DRAIN_CURRENT_THRESHOLD) {
        canvas_draw_icon(canvas, x + 16, y + 7, &I_FaceConfused_29x14);
    } else if(info->charge < LOW_CHARGE_THRESHOLD) {
        canvas_draw_icon(canvas, x + 16, y + 7, &I_FaceNopower_29x14);
    } else {
        canvas_draw_icon(canvas, x + 16, y + 7, &I_FaceNormal_29x14);
    }

    // Draw bubble
    elements_bubble(canvas, x + 53, y + 0, 71, 28);

    // Set text
    if(charge_current > 0) {
        snprintf(header, sizeof(header), "%s", "Charging at");
        snprintf(
            value,
            sizeof(value),
            "%lu.%luV   %lumA",
            (uint32_t)(info->voltage_vbus),
            (uint32_t)(info->voltage_vbus * 10) % 10,
            charge_current);
    } else if(drain_current > 0) {
        snprintf(header, sizeof(header), "%s", "Consumption is");
        snprintf(
            value,
            sizeof(value),
            "%ld %s",
            drain_current,
            drain_current > HIGH_DRAIN_CURRENT_THRESHOLD ? "mA!" : "mA");
    } else if(drain_current != 0) {
        snprintf(header, 20, "...");
    } else if(info->voltage_battery_charging < 4.2) {
        // Non-default battery charging limit, mention it
        snprintf(header, sizeof(header), "Limited to");
        snprintf(
            value,
            sizeof(value),
            "%lu.%luV",
            (uint32_t)(info->voltage_battery_charging),
            (uint32_t)(info->voltage_battery_charging * 10) % 10);
    } else {
        snprintf(header, sizeof(header), "Charged!");
    }

    if(!strcmp(value, "")) {
        canvas_draw_str_aligned(canvas, x + 92, y + 14, AlignCenter, AlignCenter, header);
    } else if(!strcmp(header, "")) {
        canvas_draw_str_aligned(canvas, x + 92, y + 14, AlignCenter, AlignCenter, value);
    } else {
        canvas_draw_str_aligned(canvas, x + 92, y + 9, AlignCenter, AlignCenter, header);
        canvas_draw_str_aligned(canvas, x + 92, y + 19, AlignCenter, AlignCenter, value);
    }
};

static void battery_info_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    PowerInfo* info = context;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    draw_battery(canvas, info, 0, 0);

    char batt_level[10];
    char temperature[10];
    char voltage[10];
    char health[10];

    snprintf(batt_level, sizeof(batt_level), "%lu%%", (uint32_t)info->charge);
    snprintf(temperature, sizeof(temperature), "%lu C", (uint32_t)info->temperature_gauge);
    snprintf(
        voltage,
        sizeof(voltage),
        "%lu.%01lu V",
        (uint32_t)info->voltage_gauge,
        (uint32_t)(info->voltage_gauge * 10) % 10UL);
    snprintf(health, sizeof(health), "%d%%", info->health);

    draw_stat(canvas, 8, 28, &I_Battery_16x16, batt_level);
    draw_stat(canvas, 40, 28, &I_Temperature_16x16, temperature);
    draw_stat(canvas, 72, 28, &I_Voltage_16x16, voltage);
    draw_stat(canvas, 104, 28, &I_Health_16x16, health);

    elements_button_left(canvas, "Back");
    elements_button_right(canvas, "Next");
}

static bool battery_info_input_callback(InputEvent* event, void* context) {
    FuriSemaphore* semaphore = context;

    bool consumed = false;
    if(event->type == InputTypeShort) {
        if(event->key == InputKeyLeft) {
            screen_index--;
            consumed = true;
        } else if(event->key == InputKeyRight) {
            screen_index++;
            consumed = true;
        } else if(event->key == InputKeyBack) {
            screen_index = -2;
            consumed = true;
        }
    }

    if(consumed) {
        furi_semaphore_release(semaphore);
    }
    return consumed;
}

const AboutDialogScreen about_screens[] = {
    product_screen,
    compliance_screen,
    address_screen,
    icon1_screen,
    icon2_screen,
    hw_version_screen,
    fw_version_screen};

const int about_screens_count = sizeof(about_screens) / sizeof(AboutDialogScreen);

int32_t about_settings_app(void* p) {
    bool battery_info = false;
    if(p && strlen(p) && !strcmp(p, "batt")) {
        battery_info = true;
    }

    Gui* gui = furi_record_open(RECORD_GUI);
    ViewDispatcher* view_dispatcher = view_dispatcher_alloc();
    const uint32_t battery_info_index = 0;
    const uint32_t empty_screen_index = 1;

    FuriSemaphore* semaphore = furi_semaphore_alloc(1, 0);
    Power* power = furi_record_open(RECORD_POWER);
    View* battery_view = view_alloc();
    view_set_context(battery_view, semaphore);
    view_set_input_callback(battery_view, battery_info_input_callback);
    view_allocate_model(battery_view, ViewModelTypeLocking, sizeof(PowerInfo));
    view_set_draw_callback(battery_view, battery_info_draw_callback);

    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    DialogMessage* message = dialog_message_alloc();
    EmptyScreen* empty_screen = empty_screen_alloc();
    DialogMessageButton screen_result;

    // draw empty screen to prevent menu flickering
    view_dispatcher_add_view(view_dispatcher, battery_info_index, battery_view);
    view_dispatcher_add_view(
        view_dispatcher, empty_screen_index, empty_screen_get_view(empty_screen));
    view_dispatcher_attach_to_gui(view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    screen_index = -1 + !battery_info;
    while(screen_index > -2) {
        if(screen_index == -1) {
            if(!battery_info) {
                break;
            }
            with_view_model(
                battery_view, PowerInfo * model, { power_get_info(power, model); }, true);
            view_dispatcher_switch_to_view(view_dispatcher, battery_info_index);
            furi_semaphore_acquire(semaphore, 2000);
        } else {
            view_dispatcher_switch_to_view(view_dispatcher, empty_screen_index);
            if(screen_index >= about_screens_count - 1) {
                dialog_message_set_buttons(message, "Back", NULL, NULL);
            } else {
                dialog_message_set_buttons(message, "Back", NULL, "Next");
            }
            screen_result = about_screens[screen_index](dialogs, message);
            if(screen_result == DialogMessageButtonLeft) {
                screen_index--;
            } else if(screen_result == DialogMessageButtonRight) {
                screen_index++;
            } else if(screen_result == DialogMessageButtonBack) {
                screen_index = -2;
            }
        }
    }

    dialog_message_free(message);
    furi_record_close(RECORD_DIALOGS);

    furi_record_close(RECORD_POWER);
    furi_semaphore_free(semaphore);
    view_dispatcher_remove_view(view_dispatcher, empty_screen_index);
    view_dispatcher_remove_view(view_dispatcher, battery_info_index);
    view_free(battery_view);
    view_dispatcher_free(view_dispatcher);
    empty_screen_free(empty_screen);
    furi_record_close(RECORD_GUI);

    return 0;
}
