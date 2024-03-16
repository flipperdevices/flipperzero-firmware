#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_bt.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>
#include <gui/elements.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/modules/variable_item_list.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <gui/scene_manager.h>
#include <gui/modules/text_box.h>


#include "uart.h"

typedef enum {
    subMenuSetup,
    subMenuScan,
    subMenuWeapon,
    subMenuConsole,
    subMenuAbout
}subMenu;


typedef enum {
    subMenuViewMain,
    subMenuViewScan,
    subMenuViewWeapon,
    subMenuViewConsole,
    subMenuViewAbout,
    subMenuViewSetup
} ViewList;


typedef enum {
    Weapon_OKLOK_UNLOCK,
    Weapon_Aerlang,
    Weapon_COUNT
} Weapon_List;



typedef struct {
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    void *view_list[6];
    NotificationApp* notification;
    VariableItemList* variable_item_list;
    uart_app *uapp;

}bleScanCtx;

typedef struct {
    signed int state_index;
    bool is_start;
} SkeletonWeaponModel;


typedef struct {
    bleScanCtx *widget;
    FuriString *s;
} SkeletonScanModel;


NotificationMessage blink_message = {
    .type = NotificationMessageTypeLedBlinkStart,
    .data.led_blink.color = LightBlue | LightGreen,
    .data.led_blink.on_time = 10,
    .data.led_blink.period = 100,
};


const NotificationSequence blink_sequence = {
    &blink_message,
    &message_do_not_reset,
    NULL,
};

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
} WorkerEvtFlags;

#define WORKER_ALL_RX_EVENTS (WorkerEvtStop | WorkerEvtRxDone)

#define UART_CH 0

#define UART_TERMINAL_TEXT_BOX_STORE_SIZE 4096

#define TAG "ble_killer"

#define MAX_DEVICE_SCAN 5

bool scanEndFlag = false;

char *device[MAX_DEVICE_SCAN];


static void start_blink(bleScanCtx* ctx) {
    // uint16_t period = delays[state->delay];
    // if(period <= 100) period += 30;
    blink_message.data.led_blink.period = 30;
    notification_message(ctx->notification, &blink_sequence);

}
static void stop_blink(bleScanCtx* ctx) {
    notification_message(ctx->notification, &sequence_blink_stop);
}



char* flipbip_strtok_r(char* s, const char* delim, char** last) {
    char* spanp;
    int c, sc;
    char* tok;
    if(s == NULL && (s = *last) == NULL) return (NULL);
    /*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
    c = *s++;
    for(spanp = (char*)delim; (sc = *spanp++) != 0;) {
        if(c == sc) goto cont;
    }
    if(c == 0) { /* no non-delimiter characters */
        *last = NULL;
        return (NULL);
    }
    tok = s - 1;
    /*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
    for(;;) {
        c = *s++;
        spanp = (char*)delim;
        do {
            if((sc = *spanp++) == c) {
                if(c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *last = s;
                return (tok);
            }
        } while(sc != 0);
    }
    /* NOTREACHED */
}

char* flipbip_strtok(char* s, const char* delim) {
    static char* last;
    return flipbip_strtok_r(s, delim, &last);
}

static void scan_console_recv(void *ctx){
    uart_app *uapp = ((bleScanCtx *)ctx)->uapp;
    if(scanEndFlag == true){
        // furi_timer_free(uapp->scan_timer);
        return;
    }

    TextBox *text_box = uapp->text_box;

    uart_terminal_uart_set_handle_rx_data_cb(
    ((uart_app *)uapp), uart_terminal_console_output_handle_rx_data_cb); // setup callback for rx thread

    text_box_set_text(text_box, furi_string_get_cstr(((uart_app *)uapp)->text_box_store));

    char recv_buf[512];
    // // memset(recv_buf, '\0', sizeof(recv_buf));
    memcpy(recv_buf, furi_string_get_cstr(((uart_app *)uapp)->text_box_store), sizeof(recv_buf));   

    if(strstr(furi_string_get_cstr(((uart_app *)uapp)->text_box_store), ";")){
        scanEndFlag = true;

        char *token = flipbip_strtok(recv_buf, ",");
        // UNUSED(token);
        int i = 0;
        while (token != NULL && i < MAX_DEVICE_SCAN) {
            device[i] = (char *)malloc(64);
            if(device[i]!=NULL){
                memcpy(device[i], token, strlen(token));
            }
            i++;
            token = flipbip_strtok(NULL, ",");
        }

        // furi_string_printf(uapp->text_box_store, "recv buf:%s,;", uapp->rx_buf);
        // TextBox* text_box = uapp->text_box;
        // text_box_set_text(text_box, furi_string_get_cstr(uapp->text_box_store));

        stop_blink((bleScanCtx *)ctx);
        view_dispatcher_switch_to_view(((bleScanCtx *)ctx)->view_dispatcher, subMenuViewScan);
        
    }

    // furi_string_printf(uapp->text_box_store, "recv buf:%s,;", uapp->rx_buf);

    // TextBox* text_box = uapp->text_box;
    // text_box_set_text(text_box, furi_string_get_cstr(uapp->text_box_store));
    
}


void uart_send_cmd(uart_app *uart, uint8_t* data, size_t len) {
    // furi_hal_uart_tx(uart_ch, data, len);
    furi_hal_serial_tx(uart->serial_handle, data, len);
}


void skeleton_submenu_callback(void* context, uint32_t index){

    bleScanCtx *ctx = (bleScanCtx *)context;

    if(index == subMenuScan){
        view_dispatcher_switch_to_view(ctx->view_dispatcher, subMenuViewScan);
    }else if(index == subMenuWeapon){
        view_dispatcher_switch_to_view(ctx->view_dispatcher, subMenuViewWeapon);
    }else if(index == subMenuConsole){
        view_dispatcher_switch_to_view(ctx->view_dispatcher, subMenuViewConsole);
    }else if(index == subMenuAbout){
        view_dispatcher_switch_to_view(ctx->view_dispatcher, subMenuViewAbout);
    }else if(index == subMenuSetup){
        view_dispatcher_switch_to_view(ctx->view_dispatcher, subMenuViewSetup);
    }
}

static void view_scan_draw_callback(Canvas* canvas, void* context) {
    // SkeletonScanModel *model = (SkeletonScanModel *)context;
    UNUSED(context);
    if(!scanEndFlag){
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 15, 30, "Press 'OK' to scan.");
        // start_blink(_ctx);
        // notification_message(_ctx->notification, &sequence_notification);
        
    }else{
        // char buf[20] = "Aerlang";
        // FuriString* xstr = furi_string_alloc();
        // furi_string_printf(xstr, "result: %s\n", buf);
        canvas_set_font(canvas, FontPrimary);
        // canvas_draw_str(canvas, 20, 15, "Result:");
        canvas_draw_str_aligned(canvas, 10, 12, AlignLeft, AlignBottom, "Result:");

        FuriString* xstr = furi_string_alloc();
        int i;
        for(i = 0;i < MAX_DEVICE_SCAN;i++){
            if(device[i] != NULL){
                furi_string_printf(xstr, "%s", device[i]);
                canvas_set_font(canvas, FontSecondary);
                canvas_draw_str(canvas, 5, 10+i*10, furi_string_get_cstr(xstr));
            }
        }

        // furi_string_printf(xstr, "%s", "notification_message(_ctx->notification, &sequence_blink_stop);\ncanvas_draw_str(canvas, 5, 10+i*10, furi_string_get_cstr(xstr));");
        // elements_scrollable_text_line(
        //     canvas,
        //     40,
        //     50,
        //     128,
        //     xstr,
        //     0,
        //     true

        // );

        // canvas_draw_str(canvas, 50, 50, furi_string_get_cstr(model->s));

        // widget_add_string_element(model->widget->view_list[subMenuViewAbout],10,20, AlignLeft, AlignTop, FontPrimary,furi_string_get_cstr(model->s));
        // widget_add_string_multiline_element(model->widget->view_list[subMenuViewAbout],10,20, AlignLeft, AlignTop, FontPrimary,furi_string_get_cstr(model->s));


        // Bus Fault
        // widget_add_text_scroll_element(
        //     model->widget->view_list[subMenuViewScan],
        //     0,
        //     0,
        //     128,
        //     64,
        //     "This is a bluetooth ble scanner and controller tool.\n---\nScan any low energy ble device around."
        // );


        furi_string_free(xstr);
    }
    // elements_button_center(canvas, scanEndFlag ? "Stop":"Start");

}

static void view_weapon_draw_callback(Canvas* canvas, void* context) {
    // UNUSED(context);
    // FURI_LOG_I(TAG, "view_weapon_draw_callback context null.");
    SkeletonWeaponModel* model = (SkeletonWeaponModel*)context;

    if(model == NULL){
        FURI_LOG_I(TAG, "view_weapon_draw_callback context null.");
        return;
    }

    FURI_LOG_I(TAG, "View index: %d", model->state_index);

    switch(model->state_index){
        case Weapon_OKLOK_UNLOCK:
            canvas_set_font(canvas, FontPrimary);

            canvas_draw_str_aligned(canvas, 124, 12, AlignRight, AlignBottom, "OKLOK unlock");

            elements_text_box(
                    canvas,
                    4,
                    30,
                    80,
                    48,
                    AlignLeft,
                    AlignTop,
                    "Press \e#OK\e# button\n"
                    "To \e#attack\e#",
                    false);
            elements_button_center(canvas, model->is_start ? "Stop":"Start");
            elements_button_right(canvas, "Next");
            break;
        case Weapon_Aerlang:
            canvas_set_font(canvas, FontPrimary);
            canvas_draw_str_aligned(canvas, 124, 12, AlignRight, AlignBottom, "Aerlang controller");
                        elements_text_box(
                    canvas,
                    4,
                    30,
                    80,
                    48,
                    AlignLeft,
                    AlignTop,
                    "Press \e#OK\e# button\n"
                    "To \e#attack\e#",
                    false);
            elements_button_center(canvas, model->is_start ? "Stop":"Start");
            elements_button_left(canvas, "Pre");
            break;
        default:
            break;
    }
    

}


static bool view_weapon_input_callback(InputEvent* input_event, void* context){
    bleScanCtx *_ctx = (bleScanCtx *)context;
    // FURI_LOG_I(TAG, "Weapon_count: %d\n", Weapon_COUNT);
    if(input_event->type == InputTypeShort){
        if(input_event->key == InputKeyRight){
            with_view_model(
                _ctx->view_list[subMenuViewWeapon],
                SkeletonWeaponModel * model,
                {
                    if(model->state_index < Weapon_COUNT-1) {
                        model->state_index++;
                    }
                },
                true);
            return true;
        }else if(input_event->key == InputKeyLeft){
            with_view_model(
                _ctx->view_list[subMenuViewWeapon],
                SkeletonWeaponModel * model,
                {
                    if(model->state_index > 0) {
                        model->state_index--;
                    }
                },
                true);
            return true;
        }else if(input_event->key == InputKeyOk){
            SkeletonWeaponModel *model = view_get_model(_ctx->view_list[subMenuViewWeapon]);
            if(!model->is_start){
                view_dispatcher_send_custom_event(_ctx->view_dispatcher, model->state_index);
                start_blink(_ctx);
            }else{
                stop_blink(_ctx);
            }
            model->is_start = !model->is_start;
            // 更新下方按钮状态
            view_dispatcher_switch_to_view(_ctx->view_dispatcher, subMenuViewWeapon);
            
        }else if(input_event->key == InputKeyBack){
            stop_blink(_ctx);
        }
    }

    return false;
}

static bool weapon_custom_event_callback(uint32_t event, void* context) {
    bleScanCtx *ctx = (bleScanCtx *)context;
    char label[20] = "weapon";

    FuriString* cmd_buf = furi_string_alloc();

    if(ctx->uapp->uart_is_init == 0){
        // view_dispatcher_send_custom_event(_ctx->view_dispatcher, 2);
        uart_init(ctx->uapp, ctx->uapp->BAUDRATE, UART_CH);
        ctx->uapp->uart_is_init = 1;
    }

    switch(event){
        case Weapon_OKLOK_UNLOCK:
            FURI_LOG_I(TAG, "Weapon custom event: Weapon_OKLOK_UNLOCK");
            // sprintf(cmd_buf, "%s,%d", label, (int)event);
            furi_string_cat_printf(cmd_buf, "%s,%d", label, (int)event);
            uart_send_cmd(ctx->uapp, (uint8_t *)furi_string_get_cstr(cmd_buf), furi_string_size(cmd_buf));
            // return true;
            break;
        case Weapon_Aerlang:
            FURI_LOG_I(TAG, "Weapon custom event: Weapon_Aerlang");
            // sprintf(cmd_buf, "%s,%d", label, (int)event);
            furi_string_cat_printf(cmd_buf, "%s,%d", label, (int)event);
            uart_send_cmd(ctx->uapp, (uint8_t *)furi_string_get_cstr(cmd_buf), furi_string_size(cmd_buf));
            // return true;
            break;
        default:
            return false;
    }
    furi_string_free(cmd_buf);
    return false;
}

static bool setup_custom_event_callback(uint32_t event, void* context) {
    bleScanCtx *ctx = (bleScanCtx *)context;
    UNUSED(ctx);
    switch(event){
        case 0:
            return true;
        default:
            return false;
    }
    return false;
}

// 自定义event的处理
static bool scan_custom_event_callback(uint32_t event, void* context) {

    bleScanCtx *ctx = (bleScanCtx *)context;
    uart_app *uapp = ctx->uapp;

    FURI_LOG_I(TAG, "scan_custom_event_callback event: %ld", event);
    float frequency;
    switch(event){
        case TEST_BTN:
            if(furi_hal_speaker_acquire(500)) {
                frequency = 200;
                furi_hal_speaker_start(frequency, 1.0);
                furi_delay_ms(100);
                furi_hal_speaker_stop();
                furi_hal_speaker_release(); 
            }
            // x = 1;

            return true;
        case START_SCAN:
            // scanEndFlag = true;
            // uint8_t cmd[20] = "scan";

            if(!scanEndFlag){

                TextBox* text_box = uapp->text_box;
                text_box_reset(text_box);
                furi_string_reset(uapp->text_box_store);
                text_box_set_text(text_box, furi_string_get_cstr(uapp->text_box_store));

                uart_send_cmd(uapp, (uint8_t*)"scan\n", 5);
                
                if(uapp->scan_timer == NULL){
                    start_blink(ctx);
                    uapp->scan_timer = furi_timer_alloc(scan_console_recv, FuriTimerTypePeriodic, ctx);
                    furi_timer_start(uapp->scan_timer, 200);
                }

            }
            
            // while(!(strlen((char *)uapp->rx_buf) > 0));

            // furi_string_printf(uapp->text_box_store, "recv buf:%s,;", uapp->rx_buf);

            // TextBox* text_box = uapp->text_box;
            // text_box_set_text(text_box, furi_string_get_cstr(uapp->text_box_store));
            // stop_blink(bleCtx);
            return true;
        case UART_INIT:
            FURI_LOG_I(TAG, "UART init by scan module, baudrate: %d", uapp->BAUDRATE);
            uart_init(uapp, uapp->BAUDRATE, UART_CH);

            return true;
        default:
            return false;
    }
    return true;
}

static bool console_custom_event_callback(uint32_t event, void* context) {
    bleScanCtx *ctx = (bleScanCtx *)context;

    char buf[20] = "";
    switch(event){
        // clean logs
        case 1:
            // text_box_reset(_ctx->uapp->text_box);
            // view_dispatcher_switch_to_view(_ctx->view_dispatcher, subMenuViewConsole);
            
            furi_string_printf(ctx->uapp->text_box_store, "%s", buf);
            text_box_set_text(ctx->uapp->text_box, furi_string_get_cstr(ctx->uapp->text_box_store));
            return true;
        case 2:
            FURI_LOG_I(TAG, "UART init by console module, baudrate: %d", ctx->uapp->BAUDRATE);
            uart_init(ctx->uapp, ctx->uapp->BAUDRATE, UART_CH);
            return true;
        default:
            return false;

    }
    return true;
}

static bool view_scan_input_callback(InputEvent* input_event, void* context) {

    bleScanCtx *_ctx = (bleScanCtx *)context;
    // uart_app *uapp = _ctx->uapp;

    if(input_event->type == InputTypeShort){
        if(input_event->key == InputKeyLeft){
            view_dispatcher_send_custom_event(_ctx->view_dispatcher, TEST_BTN);
            return true;
        }else if(input_event->key == InputKeyOk){
            view_dispatcher_send_custom_event(_ctx->view_dispatcher, START_SCAN);
            return true;
        }
    }
    return false;
}

static bool view_console_input_callback(InputEvent* input_event, void* context) {
    bleScanCtx *_ctx = (bleScanCtx *)context;
    if(input_event->type == InputTypeShort){
        if(input_event->key == InputKeyLeft){
            view_dispatcher_send_custom_event(_ctx->view_dispatcher, 42);
            return true;
        
        // clean uart log
        }else if(input_event->key == InputKeyOk){
            // view_dispatcher_switch_to_view(_ctx->view_dispatcher, subMenuViewConsole);
            view_dispatcher_send_custom_event(_ctx->view_dispatcher, 1);
            return true;
        }
    }
    return false;
}

void uart_terminal_console_output_handle_rx_data_cb(uint8_t* buf, size_t len, void* context) {
    furi_assert(context);
    uart_app* app = context;
    FuriString* new_str = furi_string_alloc();

    furi_string_cat_printf(new_str, "%s", buf);
    
    app->text_box_store_strlen += furi_string_size(new_str);
    ;
    while(app->text_box_store_strlen >= UART_TERMINAL_TEXT_BOX_STORE_SIZE - 1) {
        furi_string_right(app->text_box_store, app->text_box_store_strlen / 2);
        app->text_box_store_strlen = furi_string_size(app->text_box_store) + len;
    }

    furi_string_cat(app->text_box_store, new_str);
    // furi_string_cat(app->text_box_store, "\n");
    furi_string_free(new_str);
}

void uart_terminal_uart_set_handle_rx_data_cb(uart_app* uart, void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context)) {
        furi_assert(uart);
        uart->handle_rx_data_cb = handle_rx_data_cb;
}


static void loop_console(void *app){

    TextBox* text_box = ((uart_app *)app)->text_box;

    
    text_box_set_font(text_box, TextBoxFontText);
    text_box_set_focus(text_box, TextBoxFocusEnd);


    uart_terminal_uart_set_handle_rx_data_cb(
        ((uart_app *)app), uart_terminal_console_output_handle_rx_data_cb); // setup callback for rx thread

    text_box_set_text(text_box, furi_string_get_cstr(((uart_app *)app)->text_box_store));
}

static void view_console_enter_callback(void* context) {
    furi_assert(context);

    bleScanCtx *_ctx = (bleScanCtx *)context;

    uart_app *uapp = _ctx->uapp;

    if(uapp->uart_is_init == 0){
        view_dispatcher_send_custom_event(_ctx->view_dispatcher, 2);
        uapp->uart_is_init = 1;
    }
    

    TextBox* text_box = uapp->text_box;
    // text_box_reset(text_box);

    text_box_set_font(text_box, TextBoxFontText);
    text_box_set_focus(text_box, TextBoxFocusEnd);

    furi_string_reset(uapp->text_box_store);

    // uart_terminal_uart_set_handle_rx_data_cb(
    //     ((uart_app *)uapp), uart_terminal_console_output_handle_rx_data_cb); // setup callback for rx thread
    text_box_set_text(text_box, furi_string_get_cstr(uapp->text_box_store));
    
    // 设置定时器，更新接收到的数据
    uapp->console_timer = furi_timer_alloc(loop_console, FuriTimerTypePeriodic, uapp);
    furi_timer_start(uapp->console_timer, 200);


}

void view_console_exit_callback(void* context) {
    bleScanCtx *_ctx = (bleScanCtx *)context;

    // UNUSED(_ctx);
    // Unregister rx callback
    uart_terminal_uart_set_handle_rx_data_cb(_ctx->uapp, NULL);
}


static void view_scan_enter_callback(void* context) {
    bleScanCtx *_ctx = (bleScanCtx *)context;
    uart_app *uapp = _ctx->uapp;
    
    if(uapp->uart_is_init == 0){
        view_dispatcher_send_custom_event(_ctx->view_dispatcher, UART_INIT);
        uapp->uart_is_init = 1;
    }
}

static void view_scan_exit_callback(void* context) {
    bleScanCtx *_ctx = (bleScanCtx *)context;
    stop_blink(_ctx);
}


static uint32_t navigation_submenu_callback(void* _context) {
    UNUSED(_context);
    return subMenuViewMain;
}

static uint32_t navigation_exit_callback(void* _context) {
    UNUSED(_context);
    return VIEW_NONE;
}



static int32_t uart_worker(void* context) {
    uart_app* uart = (void*)context;

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_ALL_RX_EVENTS, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);
        if(events & WorkerEvtStop) break;
        if(events & WorkerEvtRxDone) {
            size_t len = furi_stream_buffer_receive(uart->rx_stream, uart->rx_buf, RX_BUF_SIZE, 0);
            if(len > 0) {
                if(uart->handle_rx_data_cb) uart->handle_rx_data_cb(uart->rx_buf, len, uart);
            }
        }
    }

    furi_stream_buffer_free(uart->rx_stream);

    return 0;
}


void uart_terminal_uart_on_irq_cb(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxEvent event,
    void* context) {
    uart_app* uart = (void*)context;

    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        furi_stream_buffer_send(uart->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtRxDone);
    }
}

void uart_init(uart_app* uapp, int baudrate, FuriHalSerialId ch){
    FURI_LOG_I(TAG, "UART initialing...\n");
    uapp->rx_stream = furi_stream_buffer_alloc(320*3, 1);
    uapp->rx_thread = furi_thread_alloc();
    furi_thread_set_name(uapp->rx_thread, "UART_TerminalUartRxThread");
    furi_thread_set_stack_size(uapp->rx_thread, 1024);
    furi_thread_set_context(uapp->rx_thread, uapp);
    furi_thread_set_callback(uapp->rx_thread, uart_worker);

    furi_thread_start(uapp->rx_thread);

    uapp->BAUDRATE = baudrate;

    uapp->serial_handle = furi_hal_serial_control_acquire(ch);
    // furi_check(uapp->serial_handle);

    furi_hal_serial_init(uapp->serial_handle, uapp->BAUDRATE);

    furi_hal_serial_async_rx_start(uapp->serial_handle, uart_terminal_uart_on_irq_cb, uapp, false);

}

void uart_terminal_uart_free(uart_app* uart) {
    furi_assert(uart);

    furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtStop);
    furi_thread_join(uart->rx_thread);
    furi_thread_free(uart->rx_thread);

    furi_hal_serial_deinit(uart->serial_handle);
    furi_hal_serial_control_release(uart->serial_handle);

    free(uart);
}


char *BaudRate_strings[] = {"115200", "38400", "9600"};
static void baudRate_change_callback(VariableItem* item){
    furi_assert(item);
    
    bleScanCtx *bleCtx = variable_item_get_context(item);
    uart_app *uapp = bleCtx->uapp;

    int index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, BaudRate_strings[index]);
    uapp->baudrate_index = index;

    FURI_LOG_I(TAG, "BaudRate changed: %s", BaudRate_strings[uapp->baudrate_index]);
    uapp->BAUDRATE = atoi(BaudRate_strings[uapp->baudrate_index]);
    // view_dispatcher_send_custom_event(bleCtx->view_dispatcher, 0);

}

bleScanCtx *ble_init(){
    bleScanCtx *bleCtx = malloc(sizeof(bleScanCtx));
    Gui* gui = furi_record_open(RECORD_GUI);


    bleCtx->view_dispatcher = view_dispatcher_alloc();

    view_dispatcher_enable_queue(bleCtx->view_dispatcher);
    view_dispatcher_attach_to_gui(bleCtx->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_set_event_callback_context(bleCtx->view_dispatcher, bleCtx);


    bleCtx->notification = furi_record_open(RECORD_NOTIFICATION);

    bleCtx->submenu = submenu_alloc();

    uart_app* uapp = malloc(sizeof(uart_app));


    // 默认波特率
    uapp->BAUDRATE = 9600;

    // uapp->scanEndFlag = false;
    
    bleCtx->uapp = uapp;

    // 添加主菜单setup list列表
    submenu_add_item(
        bleCtx->submenu, "Setup", subMenuSetup, skeleton_submenu_callback, bleCtx);
    submenu_add_item(
        bleCtx->submenu, "Scan", subMenuScan, skeleton_submenu_callback, bleCtx);
    submenu_add_item(
        bleCtx->submenu, "Weapon", subMenuWeapon, skeleton_submenu_callback, bleCtx);
    submenu_add_item(
        bleCtx->submenu, "Console", subMenuConsole, skeleton_submenu_callback, bleCtx);
    submenu_add_item(
        bleCtx->submenu, "About", subMenuAbout, skeleton_submenu_callback, bleCtx);

    view_set_previous_callback(submenu_get_view(bleCtx->submenu), navigation_exit_callback);
    view_dispatcher_add_view(
        bleCtx->view_dispatcher, subMenuViewMain, submenu_get_view(bleCtx->submenu)
    );
    view_dispatcher_switch_to_view(bleCtx->view_dispatcher, subMenuViewMain);

    bleCtx->variable_item_list = variable_item_list_alloc();
    variable_item_list_reset(bleCtx->variable_item_list);
    
    VariableItem* item = variable_item_list_add(bleCtx->variable_item_list, 
                                            "BAUDRATE", // label to display
                                            COUNT_OF(BaudRate_strings), // number of choices
                                            baudRate_change_callback, // callback
                                            bleCtx); // context [use variable_item_get_context(item) to access]
    
    variable_item_set_current_value_index(item, 0);
    variable_item_set_current_value_text(item, BaudRate_strings[0]);

    view_set_previous_callback(variable_item_list_get_view(bleCtx->variable_item_list), navigation_submenu_callback);
    view_set_custom_callback(variable_item_list_get_view(bleCtx->variable_item_list) ,setup_custom_event_callback);
    
    
    view_dispatcher_add_view(
        bleCtx->view_dispatcher, subMenuViewSetup, variable_item_list_get_view(bleCtx->variable_item_list)
    );

    
    // bleCtx->view_list[subMenuViewSetup] = view_alloc();
    bleCtx->view_list[subMenuViewScan] = widget_alloc();
    bleCtx->view_list[subMenuViewWeapon] = view_alloc();
    uapp->text_box = text_box_alloc();
    bleCtx->view_list[subMenuViewConsole] = uapp->text_box;
    // bleCtx->view_list[subMenuViewConsole] = view_alloc();
    bleCtx->view_list[subMenuViewAbout] = widget_alloc();



    View *text_box_view = text_box_get_view(uapp->text_box);
    view_dispatcher_add_view(
        bleCtx->view_dispatcher,
        subMenuViewConsole,
        text_box_view
    );

    uapp->text_box_store = furi_string_alloc();
    furi_string_reserve(uapp->text_box_store, UART_TERMINAL_TEXT_BOX_STORE_SIZE); 

    View *scan_view = widget_get_view(bleCtx->view_list[subMenuViewScan]);

    view_dispatcher_add_view(
        bleCtx->view_dispatcher,
        subMenuViewScan,
        scan_view
    );
    
    view_set_context(scan_view, bleCtx);

        
    view_set_draw_callback(scan_view, view_scan_draw_callback);
    view_set_input_callback(scan_view, view_scan_input_callback);
    view_set_previous_callback(scan_view, navigation_submenu_callback);
    view_set_enter_callback(scan_view, view_scan_enter_callback);
    view_set_exit_callback(scan_view, view_scan_exit_callback);
    view_set_custom_callback(scan_view ,scan_custom_event_callback);
    view_allocate_model(scan_view, ViewModelTypeLockFree, sizeof(SkeletonScanModel));

    SkeletonScanModel *scan_model = view_get_model(scan_view);
    scan_model->widget = bleCtx;
    scan_model->s = furi_string_alloc();
    furi_string_printf(scan_model->s, "%s", "This is a bluetooth ble scanner and controller tool.\n---\nScan any low energy ble device around.\nScan any low energy ble device around.\nScan any low energy ble device around.");


    widget_add_text_scroll_element(
        bleCtx->view_list[subMenuViewAbout],
        0,
        0,
        128,
        64,
        "This is a bluetooth ble scanner and something ble devices controller tool.\n---\nScan any low energy ble device around."
    );
    

    view_set_context(text_box_view, bleCtx);
    view_set_previous_callback(text_box_view, navigation_submenu_callback);
    view_set_enter_callback(text_box_view, view_console_enter_callback);
    view_set_exit_callback(text_box_view, view_console_exit_callback);
    view_set_input_callback(text_box_view, view_console_input_callback);
    view_set_custom_callback(text_box_view ,console_custom_event_callback);


    View *weapon_view = (View* )bleCtx->view_list[subMenuViewWeapon];


    view_dispatcher_add_view(
        bleCtx->view_dispatcher,
        subMenuViewWeapon,
        weapon_view
    );


    view_set_context(weapon_view, bleCtx);
    view_set_draw_callback(weapon_view, view_weapon_draw_callback);
    view_set_input_callback(weapon_view, view_weapon_input_callback);
    view_set_custom_callback(weapon_view ,weapon_custom_event_callback);
    // view_set_exit_callback(weapon_view, view_console_exit_callback);
    view_set_previous_callback(weapon_view, navigation_submenu_callback);
    
    // draw callback 必须使用view_allocate_model
    view_allocate_model(weapon_view, ViewModelTypeLockFree, sizeof(SkeletonWeaponModel));
    SkeletonWeaponModel* model = view_get_model(weapon_view);

    model->state_index = 0;
    model->is_start = false;


    view_set_previous_callback(widget_get_view(bleCtx->view_list[subMenuViewAbout]), navigation_submenu_callback);

    view_dispatcher_add_view(
        bleCtx->view_dispatcher,
        subMenuViewAbout,
        widget_get_view(bleCtx->view_list[subMenuViewAbout])
    );

    return bleCtx;
}



void ble_free(bleScanCtx *bleCtx){
    uart_app * uapp = bleCtx->uapp;
    view_dispatcher_remove_view(bleCtx->view_dispatcher, subMenuViewMain); 
    view_dispatcher_remove_view(bleCtx->view_dispatcher, subMenuViewScan); 
    view_dispatcher_remove_view(bleCtx->view_dispatcher, subMenuViewWeapon); 
    view_dispatcher_remove_view(bleCtx->view_dispatcher, subMenuViewConsole); 
    view_dispatcher_remove_view(bleCtx->view_dispatcher, subMenuViewAbout);
    view_dispatcher_remove_view(bleCtx->view_dispatcher, subMenuViewSetup);

    int i;
    for(i = 0;i < MAX_DEVICE_SCAN;i++){
        if(device[i] != NULL){
            free(device[i]);
        }
    }


    if(uapp->console_timer){
        furi_timer_free(uapp->console_timer);
    }

    if(uapp->scan_timer){
        furi_timer_free(uapp->scan_timer);
    }

    text_box_free(uapp->text_box);
    furi_string_free(uapp->text_box_store);

    if(uapp->uart_is_init != 0){
        uart_terminal_uart_free(uapp);
    }

    widget_free(bleCtx->view_list[subMenuViewAbout]);
    // widget_free(bleCtx->view_list[subMenuViewScan]);
    
    submenu_free(bleCtx->submenu);  

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    
    view_dispatcher_free(bleCtx->view_dispatcher);

    free(bleCtx);
    scanEndFlag = false;
}

int32_t main_entry() {

    bleScanCtx *bleCtx = ble_init();

    FURI_LOG_I(TAG, "FINISH ble_init.");

    view_dispatcher_run(bleCtx->view_dispatcher);

    ble_free(bleCtx);

    return 0;
}

