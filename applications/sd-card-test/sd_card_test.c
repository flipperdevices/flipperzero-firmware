#include "u8g2/u8g2.h"
#include "fatfs/ff.h"
#include "flipper.h"
#include "flipper_v2.h"
#include <stdio.h>

extern uint8_t BSP_SD_Init();

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    union {
        InputEvent input;
    } value;
    EventType type;
} AppEvent;

typedef enum {
    StepLoading,
    StepWaitForCard,
    StepWaitForOK,
} Steps;

#define LINES_COUNT 6
typedef struct {
    Steps step_id;
    char* line[LINES_COUNT];
} State;

static void input_callback(InputEvent* input_event, void* ctx) {
    osMessageQueueId_t event_queue = (QueueHandle_t)ctx;

    AppEvent event;
    event.type = EventTypeKey;
    event.value.input = *input_event;
    osMessageQueuePut(event_queue, &event, 0, 0);
}

static void render_callback(CanvasApi* canvas, void* ctx) {
    State* state = (State*)acquire_mutex((ValueMutex*)ctx, 25);

    canvas->clear(canvas);
    canvas->set_color(canvas, ColorBlack);
    canvas->set_font(canvas, FontSecondary);
    for(uint8_t i = 0; i < LINES_COUNT; i++) {
        canvas->draw_str(canvas, 0, (i + 1) * 10, state->line[i]);
    }

    release_mutex((ValueMutex*)ctx, state);
}

void set_text(
    State* state,
    char* line1,
    char* line2,
    char* line3,
    char* line4,
    char* line5,
    char* line6) {
    state->line[0] = line1;
    state->line[1] = line2;
    state->line[2] = line3;
    state->line[3] = line4;
    state->line[4] = line5;
    state->line[5] = line6;
    printf("%s\n%s\n%s\n%s\n%s\n%s\n", line1, line2, line3, line4, line5, line6);
}

char* fatfs_error_desc(FRESULT res) {
    switch(res) {
    case FR_OK:
        return "ok";
        break;
    case FR_DISK_ERR:
        return "low level error";
        break;
    case FR_INT_ERR:
        return "internal error";
        break;
    case FR_NOT_READY:
        return "not ready";
        break;
    case FR_NO_FILE:
        return "no file";
        break;
    case FR_NO_PATH:
        return "no path";
        break;
    case FR_INVALID_NAME:
        return "invalid name";
        break;
    case FR_DENIED:
        return "denied";
        break;
    case FR_EXIST:
        return "already exist";
        break;
    case FR_INVALID_OBJECT:
        return "invalid file/dir obj";
        break;
    case FR_WRITE_PROTECTED:
        return "write protected";
        break;
    case FR_INVALID_DRIVE:
        return "invalid drive";
        break;
    case FR_NOT_ENABLED:
        return "no work area in volume";
        break;
    case FR_NO_FILESYSTEM:
        return "no valid FS volume";
        break;
    case FR_MKFS_ABORTED:
        return "aborted, any problem";
        break;
    case FR_TIMEOUT:
        return "timeout";
        break;
    case FR_LOCKED:
        return "file locked";
        break;
    case FR_NOT_ENOUGH_CORE:
        return "not enough core memory";
        break;
    case FR_TOO_MANY_OPEN_FILES:
        return "too many open files";
        break;
    case FR_INVALID_PARAMETER:
        return "invalid parameter";
        break;

    default:
        return "unknown error";
        break;
    }
}

void wait_for_button(osMessageQueueId_t event_queue, AppEvent* event, Input input_button) {
    osMessageQueueReset(event_queue);
    while(1) {
        osMessageQueueGet(event_queue, event, NULL, osWaitForever);

        if(event->type == EventTypeKey) {
            if(event->value.input.state == true) {
                if(event->value.input.input == input_button) {
                    break;
                }
            }
        }
    }
}

void sd_card_test_error(
    ValueMutex* state_mutex,
    Widget* widget,
    char* error_line1,
    char* error_line2) {
    State* state = (State*)acquire_mutex_block(state_mutex);
    set_text(state, error_line1, error_line2, "", "", "", "");
    state->step_id = StepWaitForOK;
    release_mutex(state_mutex, state);
    widget_update(widget);
    while(1) {
    }
}

void sd_card_test(void* p) {
    osMessageQueueId_t event_queue = osMessageQueueNew(1, sizeof(AppEvent), NULL);
    State _state;
    State* state;

    uint8_t bsp_result;
    FRESULT result;
    DIR dir;
    FILINFO fno;
    AppEvent event;

    const uint8_t str_buffer_size = 26;
    char str_buffer[LINES_COUNT][str_buffer_size];

    // TODO this should be in the target driver
    FATFS SD_FatFs;
    FATFS* fs;
    char SD_Path[6];

    printf("[fatfs test] start!\n");

    _state.step_id = StepLoading;
    for(uint8_t i = 0; i < LINES_COUNT; i++) {
        _state.line[i] = "";
    }

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, &_state, sizeof(State))) {
        printf("cannot create mutex\n");
        furiac_exit(NULL);
    }
    Widget* widget = widget_alloc();

    widget_draw_callback_set(widget, render_callback, &state_mutex);
    widget_input_callback_set(widget, input_callback, event_queue);

    // Open GUI and register widget
    GuiApi* gui = (GuiApi*)furi_open("gui");
    if(gui == NULL) {
        printf("gui is not available\n");
        furiac_exit(NULL);
    }
    gui->add_widget(gui, widget, GuiLayerFullscreen);

    state = (State*)acquire_mutex_block(&state_mutex);
    set_text(state, "Waiting", "insert sd card", "", "", "", "");
    state->step_id = StepWaitForCard;
    release_mutex(&state_mutex, state);

    widget_update(widget);

    // create pin
    //hal_gpio_read_sd_detect();

    state = (State*)acquire_mutex_block(&state_mutex);
    set_text(
        state,
        "!!Warning!!",
        "during the tests",
        "card will be formatted",
        "",
        "    press UP DOWN OK",
        "             to continue");
    state->step_id = StepWaitForOK;
    release_mutex(&state_mutex, state);
    widget_update(widget);

    //wait_for_button(event_queue, &event, InputUp);
    //wait_for_button(event_queue, &event, InputDown);
    //wait_for_button(event_queue, &event, InputOk);

    bsp_result = BSP_SD_Init();

    // BSP_SD_OK = 0
    if(bsp_result) {
        sd_card_test_error(&state_mutex, widget, "SD card init error", "BSP error");
    }

    result = f_mount(&SD_FatFs, (TCHAR const*)SD_Path, 1);

    if(result) {
        sd_card_test_error(&state_mutex, widget, "SD card mount error", fatfs_error_desc(result));
    }
    // ok, now we can work with sd cards

    char volume_label[128];
    DWORD serial_num;
    result = f_getlabel(SD_Path, volume_label, &serial_num);
    if(result)
        sd_card_test_error(&state_mutex, widget, "f_getfree error", fatfs_error_desc(result));

    snprintf(str_buffer[0], str_buffer_size, "Label: %s", volume_label);
    snprintf(str_buffer[1], str_buffer_size, "S/N: %lu", serial_num);

    state = (State*)acquire_mutex_block(&state_mutex);
    set_text(
        state,
        str_buffer[0],
        str_buffer[1],
        "",
        "",
        "             press OK",
        "             to continue");
    state->step_id = StepWaitForOK;
    release_mutex(&state_mutex, state);
    widget_update(widget);

    wait_for_button(event_queue, &event, InputOk);

    DWORD free_clusters, free_sectors, total_sectors;
    result = f_getfree(SD_Path, &free_clusters, &fs);
    if(result)
        sd_card_test_error(&state_mutex, widget, "f_getfree error", fatfs_error_desc(result));
    total_sectors = (fs->n_fatent - 2) * fs->csize;
    free_sectors = free_clusters * fs->csize;

    snprintf(str_buffer[0], str_buffer_size, "Cluster: %d sectors", fs->csize);
    snprintf(str_buffer[1], str_buffer_size, "Sector: %d bytes", fs->ssize);
    snprintf(str_buffer[2], str_buffer_size, "%lu KB total", total_sectors / 1024 * fs->ssize);
    snprintf(str_buffer[3], str_buffer_size, "%lu KB free", free_sectors / 1024 * fs->ssize);

    state = (State*)acquire_mutex_block(&state_mutex);
    set_text(
        state,
        str_buffer[0],
        str_buffer[1],
        str_buffer[2],
        str_buffer[3],
        "             press OK",
        "             to continue");
    state->step_id = StepWaitForOK;
    release_mutex(&state_mutex, state);
    widget_update(widget);

    wait_for_button(event_queue, &event, InputOk);

    state = (State*)acquire_mutex_block(&state_mutex);
    set_text(
        state, "formatting card", "", "[++++++++++++++++++++]", "[xxxxxxxxxxxxxxxxxxxx]", "", "");
    state->step_id = StepWaitForOK;
    release_mutex(&state_mutex, state);
    widget_update(widget);

    while(1) {
        /* code */
    }

    furiac_exit(NULL);
}
