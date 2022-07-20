#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <input/input.h>

#include <notification/notification_messages.h>

#include <TOTP.h>
#include "totp_app.h"

#include <base32.h>
#include <time.h>

#include <toolbox/path.h>
#include <flipper_format/flipper_format.h>

static const char* totp_file_header = "Flipper TOTP storage";
static const uint32_t totp_file_version = 1;

typedef enum {
    TotpEventTypeTick,
    TotpEventTypeInput,
} TotpEventType;

typedef struct {
    TotpEventType type;
    InputEvent input;
    uint32_t code;
} TotpEvent;

uint8_t keyId = 0;
uint8_t keys = 3;
uint8_t* base32key[] = {
    (unsigned char*)"JBSWY3DPEHPK3PXP",
    (unsigned char*)"AMOGUSYOBABOBAAA",
    (unsigned char*)"AMOGUSAAAAAAAAAA"};
const char* keyNames[] = {"Test Key 1", "Test Key 2", "Amogus key"};

int keyLengths[] = {10, 10, 10};

static void totp_app_draw_callback(Canvas* canvas, void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    TotpEvent event;
    furi_message_queue_get(event_queue, &event, FuriWaitForever);

    uint8_t hmacKey[20];

    int timezone = -3;

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "TOTP");
    canvas_draw_str(canvas, 2, 30, keyNames[keyId]);

    //FURI_LOG_I("TOTP", "key is %s", base32key[keyId]);

    base32_decode(base32key[keyId], hmacKey, keyLengths[keyId]);
    //FURI_LOG_I("TOTP", "len = %d", len);

    //uint8_t hmacKey[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0xde, 0xad, 0xbe, 0xef}; // Secret key
    TOTP(hmacKey, keyLengths[keyId], 30); // Secret key, Secret key length, Timestep (30s)

    FuriHalRtcDateTime datetime = {0};
    furi_hal_rtc_get_datetime(&datetime);

    struct tm date = {0};
    date.tm_hour = datetime.hour + timezone;
    date.tm_min = datetime.minute;
    date.tm_sec = datetime.second;
    date.tm_mday = datetime.day;
    date.tm_mon = datetime.month - 1;
    date.tm_year = datetime.year - 1900;
    // god i hate these 5 lines

    uint32_t newCode = getCodeFromTimestamp(mktime(&date));
    //FURI_LOG_I("TOTP", "%06ld", newCode);
    char code_string[100] = "";
    sprintf(code_string, "%06ld", newCode);
    canvas_draw_str(canvas, 2, 20, code_string);
    sprintf(code_string, "%d seconds left", 29 - date.tm_sec % 30);
    canvas_draw_str(canvas, 2, 40, code_string);
    sprintf(
        code_string,
        "%02d:%02d:%02d %02d-%02d-%04d",
        datetime.hour + timezone,
        datetime.minute,
        datetime.second,
        datetime.day,
        datetime.month,
        datetime.year);
    canvas_draw_str(canvas, 2, 50, code_string);
    canvas_draw_box(canvas, 0, 52, (29 - (date.tm_sec % 30)) * 4.414, 10);
}

static void totp_app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;

    TotpEvent event = {.type = TotpEventTypeInput, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

void totp_app_update(void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    TotpEvent event = {.type = TotpEventTypeTick};
    furi_message_queue_put(event_queue, &event, 0);
}

int32_t totp_app(void* p) {
    UNUSED(p);
    bool saved = false;
    Storage* storage = furi_record_open("storage");
    FlipperFormat* file = flipper_format_file_alloc(storage);
    string_t temp_str;
    char key_name[50] = "";
    string_init(temp_str);

    do {
        // Create nfc directory if necessary
        if(!storage_simply_mkdir(storage, TOTP_APP_FOLDER)) break;
        // Open file
        string_printf(temp_str, "%s/%s%s", "/ext/totp", "keys", ".totp");
        // Open file
        if(!flipper_format_file_open_always(file, string_get_cstr(temp_str))) break;
        // Write header
        if(!flipper_format_write_header_cstr(file, totp_file_header, totp_file_version)) break;
        // Write nfc device type
        //nfc_device_prepare_format_string(dev, temp_str);

        for(int key = 0; key < keys; key++) {
            string_printf(temp_str, "%s", base32key[key]);
            sprintf(key_name, "%s", keyNames[key]);

            if(!flipper_format_write_string(file, key_name, temp_str)) break;
        }
        //if(!flipper_format_write_hex(file, "Totp key 1", base32key[0], sizeof(base32key[0])))
        //    break;
        // Write UID, ATQA, SAK
        //if(!flipper_format_write_comment_cstr(file, "UID, ATQA and SAK are common for all formats"))
        //    break;
        //if(!flipper_format_write_hex(file, "UID", data->uid, data->uid_len)) break;
        //if(!flipper_format_write_hex(file, "ATQA", data->atqa, 2)) break;
        //if(!flipper_format_write_hex(file, "SAK", &data->sak, 1)) break;
        // Save more data if necessary
        //if(dev->format == NfcDeviceSaveFormatMifareUl) {
        //    if(!nfc_device_save_mifare_ul_data(file, dev)) break;
        //} else if(dev->format == NfcDeviceSaveFormatBankCard) {
        //    if(!nfc_device_save_bank_card_data(file, dev)) break;
        //}
        saved = true;
    } while(0);
    if(saved) {
        flipper_format_free(file);
        furi_record_close("storage");
    } else {
        flipper_format_free(file);
        furi_record_close("storage");
    }

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(TotpEvent));
    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_input_callback_set(view_port, totp_app_input_callback, event_queue);
    view_port_draw_callback_set(view_port, totp_app_draw_callback, NULL);

    // Register view port in GUI
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    view_port_update(view_port);

    FuriTimer* timer = furi_timer_alloc(totp_app_update, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(timer, furi_kernel_get_tick_frequency());

    TotpEvent event;

    while(1) {
        view_port_update(view_port);
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        if((event.input.type == InputTypeShort) && (event.input.key == InputKeyBack)) {
            break;
        } else if((event.input.type == InputTypeShort) && (event.input.key == InputKeyRight)) {
            if(keyId < keys - 1) {
                keyId++;
            } else {
                keyId = 0;
            }
        } else if((event.input.type == InputTypeShort) && (event.input.key == InputKeyLeft)) {
            if(keyId > 0) {
                keyId--;
            } else {
                keyId = keys - 1;
            }
        }
    }

    furi_timer_free(timer);

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close("gui");
    return 0;
}
