// TODO
// (DONE) Fix performance when not being charged
// (DONE) Add UART command parsing to Esp32
// (DONE) Prepare application and icon on github
// (DONE) Write snapshots to SD card
// 5. Set a constant refresh rate to the Flipper's display
// 6. Emulate grayscale
// 7. Photo browser app

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/icon_i.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <gui/elements.h>
#include <furi_hal_serial_control.h>
#include <furi_hal_serial.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/dialog_ex.h>
#include <storage/filesystem_api_defines.h>
#include <storage/storage.h>

#include <assets_icons.h>

#define THREAD_ALLOC 2048

#define FRAME_WIDTH 128
#define FRAME_HEIGTH 64
#define FRAME_BIT_DEPTH 1
#define FRAME_BUFFER_LENGTH \
    (FRAME_WIDTH * FRAME_HEIGTH * FRAME_BIT_DEPTH / 8) // 128*64*1 / 8 = 1024
#define ROW_BUFFER_LENGTH (FRAME_WIDTH / 8) // 128/8 = 16
#define LAST_ROW_INDEX (FRAME_BUFFER_LENGTH - ROW_BUFFER_LENGTH) // 1024 - 16 = 1008
#define RING_BUFFER_LENGTH (ROW_BUFFER_LENGTH + 3) // ROW_BUFFER_LENGTH + Header => 16 + 3 = 19
#define BITMAP_HEADER_LENGTH 62
#define IMAGE_FILE_DIRECTORY_PATH EXT_PATH("DCIM")

static const unsigned char bitmap_header[BITMAP_HEADER_LENGTH] = {
    0x42, 0x4D, 0x3E, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00,
    0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00};

typedef struct UartDumpModel UartDumpModel;

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    View* view;
    FuriThread* worker_thread;
    FuriStreamBuffer* rx_stream;
    FuriHalSerialHandle* serial_handle;
} UartEchoApp;

struct UartDumpModel {
    uint8_t pixels[FRAME_BUFFER_LENGTH];

    bool initialized;
    //bool marauderInitialized;
    uint8_t row_ringbuffer[RING_BUFFER_LENGTH];
    uint8_t ringbuffer_index;
};

typedef enum {
    WorkerEventReserved = (1 << 0), // Reserved for StreamBuffer internal event
    WorkerEventStop = (1 << 1),
    WorkerEventRx = (1 << 2),
} WorkerEventFlags;

#define WORKER_EVENTS_MASK (WorkerEventStop | WorkerEventRx)

const NotificationSequence sequence_notification = {
    &message_display_backlight_on,
    &message_delay_10,
    NULL,
};