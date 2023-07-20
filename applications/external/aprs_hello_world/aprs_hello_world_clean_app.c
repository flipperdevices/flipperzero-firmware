#include "aprs_hello_world_clean_app.h"

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/elements.h>

#include <input/input.h>

#include <furi_hal_random.h>

#define _1200 1
#define _2400 0

#define _FLAG 0x7e
#define _CTRL_ID 0x03
#define _PID 0xf0
#define _DT_EXP ','
#define _DT_STATUS '>'
#define _DT_POS '!'

#define _FIXPOS 1
#define _STATUS 2
#define _FIXPOS_STATUS 3

bool nada = _2400;

/*
 * SQUARE WAVE SIGNAL GENERATION
 * 
 * baud_adj lets you to adjust or fine tune overall baud rate
 * by simultaneously adjust the 1200 Hz and 2400 Hz tone,
 * so that both tone would scales synchronously.
 * adj_1200 determined the 1200 hz tone adjustment.
 * tc1200 is the half of the 1200 Hz signal periods.
 * 
 *      -------------------------                           -------
 *     |                         |                         |
 *     |                         |                         |
 *     |                         |                         |
 * ----                           -------------------------
 * 
 *     |<------ tc1200 --------->|<------ tc1200 --------->|
 *     
 * adj_2400 determined the 2400 hz tone adjustment.
 * tc2400 is the half of the 2400 Hz signal periods.
 * 
 *      ------------              ------------              -------
 *     |            |            |            |            |
 *     |            |            |            |            |            
 *     |            |            |            |            |
 * ----              ------------              ------------
 * 
 *     |<--tc2400-->|<--tc2400-->|<--tc2400-->|<--tc2400-->|
 *     
 */

const float baud_adj = 0.99;
const float adj_1200 = 1.0 * baud_adj;
const float adj_2400 = 1.0 * baud_adj;
unsigned int tc1200 = (unsigned int)(0.5 * adj_1200 * 1000000.0 / 1200.0);
unsigned int tc2400 = (unsigned int)(0.5 * adj_2400 * 1000000.0 / 2400.0);

//---------------------------------------------------------------------------

const char* mycall = "MYCALL";
char myssid = 1;

const char* dest = "APRS";

const char* digi = "WIDE2";
char digissid = 1;

const char* mystatus = "Hello World! This is my simple Flipper Zero APRS Transmitter!";

const char* lat = "55.0N";
const char* lon = "37.0E";
const char sym_ovl = 'H';
const char sym_tab = 'a';

//---------------------------------------------------------------------------
/*
 * This strings will be used to generate AFSK signals, over and over again.
 */
const char strings[860] = {
    "b0NWtAXLKj0Sn8WRsakzQS8JN25zAAf3md5ILaYty6jvZHrq1QU1CWfC6tKOMY7cFCopla9sn0b\
n26zcd9qRHFWflqMcmMwx9ZDmzxrs4cfjiMox4R0pNCB0fm26gDVcdMCZcVOnovLDWUlFHL0m2UL\
j3SVJonE4swIlemv2miVFJ3hjETh54cubpJhefhHtOGlwwtd64PigxsjzB3oXI6tJR3sCd84sheQ\
is2DrnBZPd4pYdZvv6nx01hDeQNiUYGilAHb7cdqlEIMwhHVaqIgn43JOwQzSMGOWvAbFdSxLyoU\
d8rYeyVWHxW3tyJS7wjWjsr1UV3RCkPBL4XhMpceV3z0zu6y9rQGWxBwVAbBliOo630lkdmwRkuM\
B0INNcS4CjELYzsVQnEnX5OMCryDdbFEGwCpDEiFPETlP4EeqsYI6ACIRsM9A8buf1eecrwBKgkT\
3Ty0mHlOjc4ibBiJCJB5vTzvEbQdfgsLGubfPL1Y8Vb5PAzwCGVotWxUPUPamGgBezXZ4JbOAbUf\
XGEM1ppuRtam8zk4ePExs1ccD4qumNt0pvfEWyCiIrVuLAK1TGoOG9rE0U0wCaLILlmLiTu1UtPM\
STm1sZzEAdunENMmMrHH4bO5W3dL36Njoq7fCVyFGiIurYBcmamYRWHFas3f6DCN7IpOiKo0PM1E\
If7eeVegEB4lQZ5EVSXJ4HpGodk4h903bu4KIfm2VilJUUtjiy9lMqTXGliafDss5zBGpL8S7yh1\
z2NdgD8TrRGXR4EJ9gSiJTCBiGoSe1uzoeqPNV1pMM7ld7bKbTriOlBNyTCm7lx7cM8J5IsO4ieg\
CSjG0OzwiQEhed7hvS2b78Qu"};

//---------------------------------------------------------------------------

unsigned int tx_delay = 5000;
unsigned int str_len = 400;

//---------------------------------------------------------------------------

char bit_stuff = 0;
unsigned short crc = 0xffff;

//---------------------------------------------------------------------------

/*
 * 
 */
void set_nada_1200(APRSHelloWorldCleanApp* app);
void set_nada_2400(APRSHelloWorldCleanApp* app);
void set_nada(APRSHelloWorldCleanApp* app, bool nada);

void calc_crc(bool in_bit);
void send_crc(APRSHelloWorldCleanApp* app);

void send_char_NRZI(APRSHelloWorldCleanApp* app, unsigned char in_byte, bool enBitStuff);
void send_string_len(APRSHelloWorldCleanApp* app, const char* in_string, int len);

void send_packet(APRSHelloWorldCleanApp* app, uint8_t packet_type);
void send_flag(APRSHelloWorldCleanApp* app, unsigned char flag_len);
void send_header(APRSHelloWorldCleanApp* app);
void send_payload(APRSHelloWorldCleanApp* app, char type);

//---------------------------------------------------------------------------

void set_nada_1200(APRSHelloWorldCleanApp* app) {
    taskENTER_CRITICAL();
    app->output_value = true;
    furi_hal_gpio_write(app->output_pin, app->output_value);
    furi_delay_us(tc1200);
    app->output_value = false;
    furi_hal_gpio_write(app->output_pin, app->output_value);
    furi_delay_us(tc1200);
    taskEXIT_CRITICAL();
}

//---------------------------------------------------------------------------

void set_nada_2400(APRSHelloWorldCleanApp* app) {
    taskENTER_CRITICAL();
    app->output_value = true;
    furi_hal_gpio_write(app->output_pin, app->output_value);
    furi_delay_us(tc2400);
    app->output_value = false;
    furi_hal_gpio_write(app->output_pin, app->output_value);
    furi_delay_us(tc2400);

    app->output_value = true;
    furi_hal_gpio_write(app->output_pin, app->output_value);
    furi_delay_us(tc2400);
    app->output_value = false;
    furi_hal_gpio_write(app->output_pin, app->output_value);
    furi_delay_us(tc2400);
    taskEXIT_CRITICAL();
}

//---------------------------------------------------------------------------

void set_nada(APRSHelloWorldCleanApp* app, bool nada) {
    taskENTER_CRITICAL();
    if(nada)
        set_nada_1200(app);
    else
        set_nada_2400(app);
    taskEXIT_CRITICAL();
}

//---------------------------------------------------------------------------

/*
 * This function will calculate CRC-16 CCITT for the FCS (Frame Check Sequence)
 * as required for the HDLC frame validity check.
 * 
 * Using 0x1021 as polynomial generator. The CRC registers are initialized with
 * 0xFFFF
 */

void calc_crc(bool in_bit) {
    unsigned short xor_in;

    xor_in = crc ^ in_bit;
    crc >>= 1;

    if(xor_in & 0x01) crc ^= 0x8408;
}

//---------------------------------------------------------------------------

void send_crc(APRSHelloWorldCleanApp* app) {
    taskENTER_CRITICAL();
    unsigned char crc_lo = crc ^ 0xff;
    unsigned char crc_hi = (crc >> 8) ^ 0xff;

    send_char_NRZI(app, crc_lo, 1);
    send_char_NRZI(app, crc_hi, 1);
    taskEXIT_CRITICAL();
}

//---------------------------------------------------------------------------

void send_header(APRSHelloWorldCleanApp* app) {
    taskENTER_CRITICAL();
    char temp;

    /*
        * APRS AX.25 Header 
        * ........................................................
        * |   DEST   |  SOURCE  |   DIGI   | CTRL FLD |    PID   |
        * --------------------------------------------------------
        * |  7 bytes |  7 bytes |  7 bytes |   0x03   |   0xf0   |
        * --------------------------------------------------------
        * 
        * DEST   : 6 byte "callsign" + 1 byte ssid
        * SOURCE : 6 byte your callsign + 1 byte ssid
        * DIGI   : 6 byte "digi callsign" + 1 byte ssid
        * 
        * ALL DEST, SOURCE, & DIGI are left shifted 1 bit, ASCII format.
        * DIGI ssid is left shifted 1 bit + 1
        * 
        * CTRL FLD is 0x03 and not shifted.
        * PID is 0xf0 and not shifted.
        */

    /********* DEST ***********/
    temp = strlen(dest);
    for(int j = 0; j < temp; j++) send_char_NRZI(app, dest[j] << 1, 1);
    if(temp < 6) {
        for(int j = 0; j < (6 - temp); j++) send_char_NRZI(app, ' ' << 1, 1);
    }
    send_char_NRZI(app, '0' << 1, 1);

    /********* SOURCE *********/
    temp = strlen(mycall);
    for(int j = 0; j < temp; j++) send_char_NRZI(app, mycall[j] << 1, 1);
    if(temp < 6) {
        for(int j = 0; j < (6 - temp); j++) send_char_NRZI(app, ' ' << 1, 1);
    }
    send_char_NRZI(app, (myssid + '0') << 1, 1);

    /********* DIGI ***********/
    temp = strlen(digi);
    for(int j = 0; j < temp; j++) send_char_NRZI(app, digi[j] << 1, 1);
    if(temp < 6) {
        for(int j = 0; j < (6 - temp); j++) send_char_NRZI(app, ' ' << 1, 1);
    }
    send_char_NRZI(app, ((digissid + '0') << 1) + 1, 1);

    /***** CTRL FLD & PID *****/
    send_char_NRZI(app, _CTRL_ID, 1);
    send_char_NRZI(app, _PID, 1);
    taskEXIT_CRITICAL();
}

//---------------------------------------------------------------------------

void send_payload(APRSHelloWorldCleanApp* app, char type) {
    taskENTER_CRITICAL();
    if(type == _FIXPOS) {
        send_char_NRZI(app, _DT_POS, 1);
        send_string_len(app, lat, strlen(lat));
        send_char_NRZI(app, sym_ovl, 1);
        send_string_len(app, lon, strlen(lon));
        send_char_NRZI(app, sym_tab, 1);
    } else if(type == _STATUS) {
        notification_message(app->notifications, &blue_led_enable);

        send_char_NRZI(app, _DT_STATUS, 1);
        send_string_len(app, mystatus, strlen(mystatus));
    } else if(type == _FIXPOS_STATUS) {
        send_char_NRZI(app, _DT_POS, 1);
        send_string_len(app, lat, strlen(lat));
        send_char_NRZI(app, sym_ovl, 1);
        send_string_len(app, lon, strlen(lon));
        send_char_NRZI(app, sym_tab, 1);

        send_char_NRZI(app, ' ', 1);
        send_string_len(app, mystatus, strlen(mystatus));
    }
    taskEXIT_CRITICAL();
}

//---------------------------------------------------------------------------

/*
 * This function will send one byte input and convert it
 * into AFSK signal one bit at a time LSB first.
 * 
 * The encode which used is NRZI (Non Return to Zero, Inverted)
 * bit 1 : transmitted as no change in tone
 * bit 0 : transmitted as change in tone
 */

void send_char_NRZI(APRSHelloWorldCleanApp* app, unsigned char in_byte, bool enBitStuff) {
    taskENTER_CRITICAL();
    bool bits;

    for(int i = 0; i < 8; i++) {
        bits = in_byte & 0x01;

        calc_crc(bits);

        if(bits) {
            set_nada(app, nada);
            bit_stuff++;

            if((enBitStuff) && (bit_stuff == 5)) {
                nada ^= 1;
                set_nada(app, nada);

                bit_stuff = 0;
            }
        } else {
            nada ^= 1;
            set_nada(app, nada);

            bit_stuff = 0;
        }

        in_byte >>= 1;
    }
    taskEXIT_CRITICAL();
}

//---------------------------------------------------------------------------

void send_string_len(APRSHelloWorldCleanApp* app, const char* in_string, int len) {
    taskENTER_CRITICAL();
    for(int j = 0; j < len; j++) {
        send_char_NRZI(app, in_string[j], 1);
    }
    taskEXIT_CRITICAL();
}

//---------------------------------------------------------------------------

void send_flag(APRSHelloWorldCleanApp* app, unsigned char flag_len) {
    taskENTER_CRITICAL();
    for(int j = 0; j < flag_len; j++) send_char_NRZI(app, _FLAG, 0);
    taskEXIT_CRITICAL();
}

//---------------------------------------------------------------------------

/*
 * In this preliminary test, a packet is consists of FLAG(s) and PAYLOAD(s).
 * Standard APRS FLAG is 0x7e character sent over and over again as a packet
 * delimiter. In this example, 100 flags is used the preamble and 3 flags as
 * the postamble.
 */

void send_packet(APRSHelloWorldCleanApp* app, uint8_t packet_type) {
    notification_message(app->notifications, &red_led_enable);

    taskENTER_CRITICAL();
    /*
        * AX25 FRAME
        * 
        * ........................................................
        * |  FLAG(s) |  HEADER  | PAYLOAD  | FCS(CRC) |  FLAG(s) |
        * --------------------------------------------------------
        * |  N bytes | 22 bytes |  N bytes | 2 bytes  |  N bytes |
        * --------------------------------------------------------
        * 
        * FLAG(s)  : 0x7e
        * HEADER   : see header
        * PAYLOAD  : 1 byte data type + N byte info
        * FCS      : 2 bytes calculated from HEADER + PAYLOAD
        */

    send_flag(app, 100);

    crc = 0xffff;
    send_header(app);

    send_payload(app, packet_type);

    send_crc(app);

    send_flag(app, 3);

    taskEXIT_CRITICAL();

    notification_message(app->notifications, &red_led_disable);
}

//---------------------------------------------------------------------------

/*
 * Function to randomized the value of a variable with defined low and hi limit value.
 * Used to create random AFSK pulse length.
 */
void randomize(unsigned int* var, unsigned int low, unsigned int high) {
    *var = low + rand() % (high - low);
}

//---------------------------------------------------------------------------

static void aprs_hello_world_clean_app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;

    APRSHelloWorldCleanEvent event = {.type = EventTypeInput, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

//---------------------------------------------------------------------------

static void aprs_hello_world_clean_app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 14, 8, "This is an APRS Hello World");

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 55, 32, "RUN");

    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(
        canvas, 127, 40, AlignRight, AlignTop, "to finish, \n click back");
}

//---------------------------------------------------------------------------

static void timer_callback(FuriMessageQueue* event_queue) {
    // Проверяем, что контекст не нулевой
    furi_assert(event_queue);

    APRSHelloWorldCleanEvent event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &event, 0);
}

//---------------------------------------------------------------------------

APRSHelloWorldCleanApp* aprs_hello_world_clean_app_alloc() {
    APRSHelloWorldCleanApp* app = malloc(sizeof(APRSHelloWorldCleanApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(APRSHelloWorldCleanEvent));
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    view_port_draw_callback_set(app->view_port, aprs_hello_world_clean_app_draw_callback, NULL);
    view_port_input_callback_set(
        app->view_port, aprs_hello_world_clean_app_input_callback, app->event_queue);

    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    app->timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, app->event_queue);

    app->output_pin = &gpio_ext_pa6;

    furi_hal_gpio_init(app->output_pin, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

    return app;
}

//---------------------------------------------------------------------------

void aprs_hello_world_clean_app_free(APRSHelloWorldCleanApp* app) {
    furi_assert(app);

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    furi_timer_free(app->timer);

    furi_message_queue_free(app->event_queue);

    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
}

//---------------------------------------------------------------------------

int32_t aprs_hello_world_clean_app(void* p) {
    UNUSED(p);
    APRSHelloWorldCleanApp* app = aprs_hello_world_clean_app_alloc();

    APRSHelloWorldCleanEvent event;

    furi_timer_start(app->timer, 5000);

    //---------------------------------------------------------------------------

    while(1) {
        // Выбираем событие из очереди в переменную event (ждем бесконечно долго, если очередь пуста)
        // и проверяем, что у нас получилось это сделать
        furi_check(
            furi_message_queue_get(app->event_queue, &event, FuriWaitForever) == FuriStatusOk);

        // Наше событие — это нажатие кнопки
        if(event.type == EventTypeInput) {
            // Если нажата кнопка "назад", то выходим из цикла, а следовательно и из приложения
            if(event.input.key == InputKeyBack) {
                break;
            }
            // Наше событие — это сработавший таймер
        } else if(event.type == EventTypeTick) {
            // Сделаем что-то по таймеру
            // Отправляем нотификацию мигания синим светодиодом
            //notification_message(app->notifications, &sequence_blink_blue_100);
            send_packet(app, (1 + rand() % (4 - 1)));
        }
    }

    aprs_hello_world_clean_app_free(app);
    return 0;
}
