#include "app-template.h"
#include "fatfs/ff.h"
#include "stm32_adafruit_sd.h"

// event enumeration type
typedef uint8_t event_t;

class SdTestState {
public:
    // state data
    static const uint8_t lines_count = 6;
    const char* line[lines_count];

    // state initializer
    SdTestState() {
        for(uint8_t i = 0; i < lines_count; i++) {
            line[i] = "";
        }
    }
};

// events class
class SdTestEvent {
public:
    // events enum
    static const event_t EventTypeTick = 0;
    static const event_t EventTypeKey = 1;

    // payload
    union {
        InputEvent input;
    } value;

    // event type
    event_t type;
};

// our app derived from base AppTemplate class
// with template variables <state, events>
class SdTest : public AppTemplate<SdTestState, SdTestEvent> {
public:
    // vars
    GpioPin* red_led_record;
    GpioPin* green_led_record;
    FATFS sd_fat_fs;
    char sd_path[6];

    // funcs
    void run();
    void render(CanvasApi* canvas, SdTestState* state);
    template <class T> void set_text(std::initializer_list<T> list);
    template <class T> void set_error(std::initializer_list<T> list);
    const char* fatfs_error_desc(FRESULT res);
    void wait_for_button(Input input_button);
    void blink_red();
    void blink_green();

    // "tests"
    void detect_sd_card();
    void show_warning();
    void init_sd_card();
    void mount_sd_card();
    void get_sd_card_info();
};

// start app
void SdTest::run() {
    // create pin
    GpioPin red_led = led_gpio[0];
    GpioPin green_led = led_gpio[1];

    // TODO open record
    red_led_record = &red_led;
    green_led_record = &green_led;

    // configure pin
    gpio_init(red_led_record, GpioModeOutputOpenDrain);
    gpio_init(green_led_record, GpioModeOutputOpenDrain);

    detect_sd_card();
    show_warning();
    init_sd_card();
    mount_sd_card();
    get_sd_card_info();
    set_text({"123", "456"});
    while(1) {
    };
}

void SdTest::detect_sd_card() {
    set_text({"Waiting", "insert sd card"});
    // detect sd card pin
    /*while(!hal_gpio_read_sd_detect()) {
        delay(100);
    }*/
}

void SdTest::show_warning() {
    set_text(
        {"!!Warning!!",
         "during the tests",
         "card will be formatted",
         "",
         "    press UP DOWN OK",
         "             to continue"});

    //wait_for_button(InputUp);
    //wait_for_button(InputDown);
    //wait_for_button(InputOk);
}

void SdTest::init_sd_card() {
    uint8_t bsp_result = BSP_SD_Init();

    // BSP_SD_OK = 0
    if(bsp_result) {
        set_error({"SD card init error", "BSP error"});
    }
}

void SdTest::mount_sd_card() {
    FRESULT result;
    BYTE work[_MAX_SS * 4];

    result = f_mkfs(sd_path, (FM_FAT | FM_FAT32 | FM_EXFAT), 0, work, _MAX_SS);

    result = f_mount(&sd_fat_fs, sd_path, 1);

    if(result) {
        if(result == FR_NO_FILESYSTEM) {
            // we get sd card without volume
            set_text(
                {"!!Warning!!",
                 "no filesystem on card",
                 "card will be formatted",
                 "",
                 "               press OK",
                 "             to continue"});
            wait_for_button(InputOk);

            result = f_mkfs(sd_path, (FM_FAT | FM_FAT32 | FM_EXFAT), 0, work, _MAX_SS);

        } else {
            set_error({"SD card mount error", fatfs_error_desc(result)});
        }
    }
}

void SdTest::get_sd_card_info() {
    const uint8_t str_buffer_size = 26;
    char str_buffer[4][str_buffer_size];
    char volume_label[128];
    DWORD serial_num;
    FRESULT result;
    FATFS* fs;
    DWORD free_clusters, free_sectors, total_sectors;

    // get label and s/n
    result = f_getlabel(sd_path, volume_label, &serial_num);
    if(result) set_error({"f_getlabel error", fatfs_error_desc(result)});

    snprintf(str_buffer[0], str_buffer_size, "Label: %s", volume_label);
    snprintf(str_buffer[1], str_buffer_size, "S/N: %lu", serial_num);

    set_text(
        {(const char*)str_buffer[0],
         (const char*)str_buffer[1],
         "",
         "",
         "             press OK",
         "             to continue"});

    wait_for_button(InputOk);

    // get total and free space
    result = f_getfree(sd_path, &free_clusters, &fs);
    if(result) set_error({"f_getfree error", fatfs_error_desc(result)});

    total_sectors = (fs->n_fatent - 2) * fs->csize;
    free_sectors = free_clusters * fs->csize;

    snprintf(str_buffer[0], str_buffer_size, "Cluster: %d sectors", fs->csize);
    snprintf(str_buffer[1], str_buffer_size, "Sector: %d bytes", fs->ssize);
    snprintf(str_buffer[2], str_buffer_size, "%lu KB total", total_sectors / 1024 * fs->ssize);
    snprintf(str_buffer[3], str_buffer_size, "%lu KB free", free_sectors / 1024 * fs->ssize);

    set_text(
        {(const char*)str_buffer[0],
         (const char*)str_buffer[1],
         (const char*)str_buffer[2],
         (const char*)str_buffer[3],
         "             press OK",
         "             to continue"});

    wait_for_button(InputOk);
}

void SdTest::wait_for_button(Input input_button) {
    SdTestEvent event;
    osMessageQueueReset(event_queue);
    while(1) {
        osStatus_t result = osMessageQueueGet(event_queue, &event, NULL, osWaitForever);

        if(event.type == SdTestEvent::EventTypeKey) {
            if(event.value.input.state == true) {
                if(event.value.input.input == input_button) {
                    blink_green();
                    break;
                } else {
                    blink_red();
                }
            }
        }
    }
    osMessageQueueReset(event_queue);
}

void SdTest::blink_red() {
    gpio_write(red_led_record, 0);
    delay(50);
    gpio_write(red_led_record, 1);
}

void SdTest::blink_green() {
    gpio_write(green_led_record, 0);
    delay(50);
    gpio_write(green_led_record, 1);
}

// FatFs errors descriptions
const char* SdTest::fatfs_error_desc(FRESULT res) {
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

// set text, but with infinite loop
template <class T> void SdTest::set_error(std::initializer_list<T> list) {
    set_text(list);
    while(1) {
    };
}

// set text, sort of variadic function
template <class T> void SdTest::set_text(std::initializer_list<T> list) {
    uint8_t line_position = 0;
    acquire_state();
    printf("------------------------\n");

    // set line strings from args
    for(auto element : list) {
        state.line[line_position] = element;
        printf("%s\n", element);
        line_position++;
        if(line_position == state.lines_count) break;
    }

    // set empty lines
    for(; line_position < state.lines_count; line_position++) {
        state.line[line_position] = "";
        printf("\n");
    }

    printf("------------------------\n");
    release_state();
}

// render app
void SdTest::render(CanvasApi* canvas, SdTestState* state) {
    canvas->set_color(canvas, ColorBlack);
    canvas->set_font(canvas, FontSecondary);
    for(uint8_t i = 0; i < state->lines_count; i++) {
        canvas->draw_str(canvas, 0, (i + 1) * 10, state->line[i]);
    }
}

// app enter function
extern "C" void sd_card_test(void* p) {
    SdTest* app = new SdTest();
    app->run();
}