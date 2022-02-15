#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <input/input.h>

#include <notification/notification_messages.h>

#include <totp.h>

#include <base32.h>
#include <time.h>

typedef enum {
    TotpEventTypeTick,
    TotpEventTypeInput,
} TotpEventType;

typedef struct {
    TotpEventType type;
    InputEvent input;
    uint32_t code;
} TotpEvent;

static void totp_app_draw_callback(Canvas* canvas, void* ctx) {
    uint8_t hmacKey[20];
    //uint8_t* base32key = (unsigned char*)"DM72NP6URDCNCHLW";
    uint8_t* base32key[] = {
        (unsigned char*)"JBSWY3DPEHPK3PXP", (unsigned char*)"AMOGUSYOBABOBAAA"};
    uint8_t keyLength = 10;
    uint8_t keyId = 0;

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "TOTP");
    canvas_draw_str(canvas, 2, 30, "Press any button");
    canvas_draw_str(canvas, 2, 40, "to update");

    FURI_LOG_I("TOTP", "key is %s", base32key[keyId]);

    int len = base32_decode(base32key[keyId], hmacKey, keyLength);
    FURI_LOG_I("TOTP", "len = %d", len);

    //uint8_t hmacKey[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0xde, 0xad, 0xbe, 0xef}; // Secret key
    TOTP(hmacKey, keyLength, 30); // Secret key, Secret key length, Timestep (30s)

    FuriHalRtcDateTime datetime = {0};
    furi_hal_rtc_get_datetime(&datetime);

    struct tm date = {0};
    date.tm_hour = datetime.hour - 3; // GMT+3 Moscow
    date.tm_min = datetime.minute;
    date.tm_sec = datetime.second;
    date.tm_mday = datetime.day;
    date.tm_mon = datetime.month - 1;
    date.tm_year = datetime.year - 1900;
    // god i hate these 5 lines

    uint32_t newCode = getCodeFromTimestamp(mktime(&date));
    FURI_LOG_I("TOTP", "%06ld", newCode);
    char code_string[12] = "";
    sprintf(code_string, "%06ld", newCode);
    canvas_draw_str(canvas, 2, 20, code_string);
}

static void totp_app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    osMessageQueueId_t event_queue = ctx;

    TotpEvent event = {.type = TotpEventTypeInput, .input = *input_event};
    osMessageQueuePut(event_queue, &event, 0, osWaitForever);
}

void totp_app_update(void* ctx) {
    furi_assert(ctx);
    osMessageQueueId_t event_queue = ctx;
    TotpEvent event = {.type = TotpEventTypeTick};
    osMessageQueuePut(event_queue, &event, 0, 0);
}

int32_t totp_app(void* p) {
    osMessageQueueId_t event_queue = osMessageQueueNew(8, sizeof(TotpEvent), NULL);
    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_input_callback_set(view_port, totp_app_input_callback, event_queue);
    view_port_draw_callback_set(view_port, totp_app_draw_callback, NULL);

    // Register view port in GUI
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    view_port_update(view_port);

    osTimerId_t timer = osTimerNew(totp_app_update, osTimerPeriodic, event_queue, NULL);
    osTimerStart(timer, osKernelGetTickFreq());

    TotpEvent event;

    while(1) {
        view_port_update(view_port);
        furi_check(osMessageQueueGet(event_queue, &event, NULL, osWaitForever) == osOK);

        if((event.input.type == InputTypeShort) && (event.input.key == InputKeyBack)) {
            break;
        }
    }

    osTimerDelete(timer);

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    osMessageQueueDelete(event_queue);

    furi_record_close("gui");
    return 0;
}
