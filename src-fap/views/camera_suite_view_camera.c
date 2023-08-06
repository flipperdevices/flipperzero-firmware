#include "../camera_suite.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>
#include "../helpers/camera_suite_haptic.h"
#include "../helpers/camera_suite_speaker.h"
#include "../helpers/camera_suite_led.h"

static CameraSuiteViewCamera* current_instance = NULL;

struct CameraSuiteViewCamera {
    CameraSuiteViewCameraCallback callback;
    FuriStreamBuffer* rx_stream;
    FuriThread* worker_thread;
    View* view;
    void* context;
};

void camera_suite_view_camera_set_callback(
    CameraSuiteViewCamera* instance,
    CameraSuiteViewCameraCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

// Function to draw pixels on the canvas based on camera orientation
static void draw_pixel_by_orientation(Canvas* canvas, uint8_t x, uint8_t y, uint8_t orientation) {
    switch(orientation) {
    case 0: // Camera rotated 0 degrees (right side up, default)
        canvas_draw_dot(canvas, x, y);
        break;
    case 1: // Camera rotated 90 degrees
        canvas_draw_dot(canvas, y, FRAME_WIDTH - 1 - x);
        break;
    case 2: // Camera rotated 180 degrees (upside down)
        canvas_draw_dot(canvas, FRAME_WIDTH - 1 - x, FRAME_HEIGHT - 1 - y);
        break;
    case 3: // Camera rotated 270 degrees
        canvas_draw_dot(canvas, FRAME_HEIGHT - 1 - y, x);
        break;
    default:
        break;
    }
}

static void camera_suite_view_camera_draw(Canvas* canvas, void* _model) {
    UartDumpModel* model = _model;

    // Clear the screen.
    canvas_set_color(canvas, ColorBlack);

    // Draw the frame.
    canvas_draw_frame(canvas, 0, 0, FRAME_WIDTH, FRAME_HEIGHT);

    CameraSuite* app = current_instance->context;

    for(size_t p = 0; p < FRAME_BUFFER_LENGTH; ++p) {
        uint8_t x = p % ROW_BUFFER_LENGTH; // 0 .. 15
        uint8_t y = p / ROW_BUFFER_LENGTH; // 0 .. 63

        for(uint8_t i = 0; i < 8; ++i) {
            if((model->pixels[p] & (1 << (7 - i))) != 0) {
                draw_pixel_by_orientation(canvas, (x * 8) + i, y, app->orientation);
            }
        }
    }

    // Draw the guide if the camera is not initialized.
    if(!model->initialized) {
        canvas_draw_icon(canvas, 74, 16, &I_DolphinCommon_56x48);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 8, 12, "Connect the ESP32-CAM");
        canvas_draw_str(canvas, 20, 24, "VCC - 3V3");
        canvas_draw_str(canvas, 20, 34, "GND - GND");
        canvas_draw_str(canvas, 20, 44, "U0R - TX");
        canvas_draw_str(canvas, 20, 54, "U0T - RX");
    }
}

static void save_image(void* _model) {
    UartDumpModel* model = _model;

    // This pointer is used to access the storage.
    Storage* storage = furi_record_open(RECORD_STORAGE);

    // This pointer is used to access the filesystem.
    File* file = storage_file_alloc(storage);

    // Store path in local variable.
    const char* folderName = EXT_PATH("DCIM");

    // Create the folder name for the image file if it does not exist.
    if(storage_common_stat(storage, folderName, NULL) == FSE_NOT_EXIST) {
        storage_simply_mkdir(storage, folderName);
    }

    // This pointer is used to access the file name.
    FuriString* file_name = furi_string_alloc();

    // Get the current date and time.
    FuriHalRtcDateTime datetime = {0};
    furi_hal_rtc_get_datetime(&datetime);

    // Create the file name.
    furi_string_printf(
        file_name,
        EXT_PATH("DCIM/%.4d%.2d%.2d-%.2d%.2d%.2d.bmp"),
        datetime.year,
        datetime.month,
        datetime.day,
        datetime.hour,
        datetime.minute,
        datetime.second);

    // Open the file for writing. If the file does not exist (it shouldn't),
    // create it.
    bool result =
        storage_file_open(file, furi_string_get_cstr(file_name), FSAM_WRITE, FSOM_OPEN_ALWAYS);

    // Free the file name after use.
    furi_string_free(file_name);

    // If the file was opened successfully, write the bitmap header and the
    // image data.
    if(result) {
        storage_file_write(file, bitmap_header, BITMAP_HEADER_LENGTH);
        int8_t row_buffer[ROW_BUFFER_LENGTH];
        for(size_t i = 64; i > 0; --i) {
            for(size_t j = 0; j < ROW_BUFFER_LENGTH; ++j) {
                row_buffer[j] = model->pixels[((i - 1) * ROW_BUFFER_LENGTH) + j];
            }
            storage_file_write(file, row_buffer, ROW_BUFFER_LENGTH);
        }
    }

    // Close the file.
    storage_file_close(file);

    // Freeing up memory.
    storage_file_free(file);
}

static void camera_suite_view_camera_model_init(UartDumpModel* const model) {
    for(size_t i = 0; i < FRAME_BUFFER_LENGTH; i++) {
        model->pixels[i] = 0;
    }
}

static bool camera_suite_view_camera_input(InputEvent* event, void* context) {
    furi_assert(context);
    CameraSuiteViewCamera* instance = context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        default: // Stop all sounds, reset the LED.
            with_view_model(
                instance->view,
                UartDumpModel * model,
                {
                    UNUSED(model);
                    camera_suite_play_bad_bump(instance->context);
                    camera_suite_stop_all_sound(instance->context);
                    camera_suite_led_set_rgb(instance->context, 0, 0, 0);
                },
                true);
            break;
        }
    } else if(event->type == InputTypePress) {
        uint8_t data[1];
        switch(event->key) {
        case InputKeyBack:
            // Stop the camera stream.
            data[0] = 's';
            // Go back to the main menu.
            with_view_model(
                instance->view,
                UartDumpModel * model,
                {
                    UNUSED(model);
                    instance->callback(CameraSuiteCustomEventSceneCameraBack, instance->context);
                },
                true);
            break;
        case InputKeyLeft:
            // Camera: Invert.
            data[0] = '<';
            with_view_model(
                instance->view,
                UartDumpModel * model,
                {
                    UNUSED(model);
                    camera_suite_play_happy_bump(instance->context);
                    camera_suite_play_input_sound(instance->context);
                    camera_suite_led_set_rgb(instance->context, 0, 0, 255);
                    instance->callback(CameraSuiteCustomEventSceneCameraLeft, instance->context);
                },
                true);
            break;
        case InputKeyRight:
            // Camera: Enable/disable dithering.
            data[0] = '>';
            with_view_model(
                instance->view,
                UartDumpModel * model,
                {
                    UNUSED(model);
                    camera_suite_play_happy_bump(instance->context);
                    camera_suite_play_input_sound(instance->context);
                    camera_suite_led_set_rgb(instance->context, 0, 0, 255);
                    instance->callback(CameraSuiteCustomEventSceneCameraRight, instance->context);
                },
                true);
            break;
        case InputKeyUp:
            // Camera: Increase contrast.
            data[0] = 'C';
            with_view_model(
                instance->view,
                UartDumpModel * model,
                {
                    UNUSED(model);
                    camera_suite_play_happy_bump(instance->context);
                    camera_suite_play_input_sound(instance->context);
                    camera_suite_led_set_rgb(instance->context, 0, 0, 255);
                    instance->callback(CameraSuiteCustomEventSceneCameraUp, instance->context);
                },
                true);
            break;
        case InputKeyDown:
            // Camera: Reduce contrast.
            data[0] = 'c';
            with_view_model(
                instance->view,
                UartDumpModel * model,
                {
                    UNUSED(model);
                    camera_suite_play_happy_bump(instance->context);
                    camera_suite_play_input_sound(instance->context);
                    camera_suite_led_set_rgb(instance->context, 0, 0, 255);
                    instance->callback(CameraSuiteCustomEventSceneCameraDown, instance->context);
                },
                true);
            break;
        case InputKeyOk: {
            CameraSuite* app = current_instance->context;
            // If flash is enabled, flash the onboard ESP32-CAM LED.
            if(app->flash) {
                data[0] = 'P';
                // Initialize the ESP32-CAM onboard torch immediately.
                furi_hal_uart_tx(FuriHalUartIdUSART1, data, 1);
                // Delay for 500ms to make sure flash is on before taking picture.
                furi_delay_ms(500);
            }
            // Take picture.
            with_view_model(
                instance->view,
                UartDumpModel * model,
                {
                    camera_suite_play_happy_bump(instance->context);
                    camera_suite_play_input_sound(instance->context);
                    camera_suite_led_set_rgb(instance->context, 0, 0, 255);
                    save_image(model);
                    instance->callback(CameraSuiteCustomEventSceneCameraOk, instance->context);
                },
                true);
            return true;
        }
        case InputKeyMAX:
            break;
        }
        // Send `data` to the ESP32-CAM
        furi_hal_uart_tx(FuriHalUartIdUSART1, data, 1);
    }
    return true;
}

static void camera_suite_view_camera_exit(void* context) {
    furi_assert(context);
}

static void camera_suite_view_camera_enter(void* context) {
    // Check `context` for null. If it is null, abort program, else continue.
    furi_assert(context);

    // Cast `context` to `CameraSuiteViewCamera*` and store it in `instance`.
    CameraSuiteViewCamera* instance = (CameraSuiteViewCamera*)context;

    // Assign the current instance to the global variable
    current_instance = instance;

    uint8_t data[1];
    data[0] = 'S'; // Uppercase `S` to start the camera

    // Send `data` to the ESP32-CAM
    furi_hal_uart_tx(FuriHalUartIdUSART1, data, 1);

    // Delay for 50ms to make sure the camera is started before sending any other commands.
    furi_delay_ms(50);

    // Initialize the camera with the selected dithering option from options.
    CameraSuite* instanceContext = instance->context;
    switch(instanceContext->dither) {
    case 0: // Floyd Steinberg
        data[0] = '0';
        break;
    case 1: // Stucki
        data[0] = '1';
        break;
    case 2: // Jarvis Judice Ninke
        data[0] = '2';
        break;
    }

    // Send `data` to the ESP32-CAM
    furi_hal_uart_tx(FuriHalUartIdUSART1, data, 1);

    with_view_model(
        instance->view,
        UartDumpModel * model,
        { camera_suite_view_camera_model_init(model); },
        true);
}

static void camera_on_irq_cb(UartIrqEvent uartIrqEvent, uint8_t data, void* context) {
    // Check `context` for null. If it is null, abort program, else continue.
    furi_assert(context);

    // Cast `context` to `CameraSuiteViewCamera*` and store it in `instance`.
    CameraSuiteViewCamera* instance = context;

    // If `uartIrqEvent` is `UartIrqEventRXNE`, send the data to the
    // `rx_stream` and set the `WorkerEventRx` flag.
    if(uartIrqEvent == UartIrqEventRXNE) {
        furi_stream_buffer_send(instance->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(instance->worker_thread), WorkerEventRx);
    }
}

static void process_ringbuffer(UartDumpModel* model, uint8_t byte) {
    // First char has to be 'Y' in the buffer.
    if(model->ringbuffer_index == 0 && byte != 'Y') {
        return;
    }

    // Second char has to be ':' in the buffer or reset.
    if(model->ringbuffer_index == 1 && byte != ':') {
        model->ringbuffer_index = 0;
        process_ringbuffer(model, byte);
        return;
    }

    // Assign current byte to the ringbuffer.
    model->row_ringbuffer[model->ringbuffer_index] = byte;
    // Increment the ringbuffer index.
    ++model->ringbuffer_index;

    // Let's wait 'till the buffer fills.
    if(model->ringbuffer_index < RING_BUFFER_LENGTH) {
        return;
    }

    // Flush the ringbuffer to the framebuffer.
    model->ringbuffer_index = 0; // Reset the ringbuffer
    model->initialized = true; // Established the connection successfully.
    size_t row_start_index =
        model->row_ringbuffer[2] * ROW_BUFFER_LENGTH; // Third char will determine the row number

    if(row_start_index > LAST_ROW_INDEX) { // Failsafe
        row_start_index = 0;
    }

    for(size_t i = 0; i < ROW_BUFFER_LENGTH; ++i) {
        model->pixels[row_start_index + i] =
            model->row_ringbuffer[i + 3]; // Writing the remaining 16 bytes into the frame buffer
    }
}

static int32_t camera_worker(void* context) {
    furi_assert(context);
    CameraSuiteViewCamera* instance = context;

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_EVENTS_MASK, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);

        if(events & WorkerEventStop) {
            break;
        } else if(events & WorkerEventRx) {
            size_t length = 0;
            do {
                size_t intended_data_size = 64;
                uint8_t data[intended_data_size];
                length =
                    furi_stream_buffer_receive(instance->rx_stream, data, intended_data_size, 0);

                if(length > 0) {
                    with_view_model(
                        instance->view,
                        UartDumpModel * model,
                        {
                            for(size_t i = 0; i < length; i++) {
                                process_ringbuffer(model, data[i]);
                            }
                        },
                        false);
                }
            } while(length > 0);

            with_view_model(
                instance->view, UartDumpModel * model, { UNUSED(model); }, true);
        }
    }

    return 0;
}

CameraSuiteViewCamera* camera_suite_view_camera_alloc() {
    CameraSuiteViewCamera* instance = malloc(sizeof(CameraSuiteViewCamera));

    instance->view = view_alloc();

    instance->rx_stream = furi_stream_buffer_alloc(2048, 1);

    // Set up views
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(UartDumpModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)camera_suite_view_camera_draw);
    view_set_input_callback(instance->view, camera_suite_view_camera_input);
    view_set_enter_callback(instance->view, camera_suite_view_camera_enter);
    view_set_exit_callback(instance->view, camera_suite_view_camera_exit);

    with_view_model(
        instance->view,
        UartDumpModel * model,
        { camera_suite_view_camera_model_init(model); },
        true);

    instance->worker_thread = furi_thread_alloc_ex("UsbUartWorker", 2048, camera_worker, instance);
    furi_thread_start(instance->worker_thread);

    // Enable uart listener
    furi_hal_console_disable();
    furi_hal_uart_set_br(FuriHalUartIdUSART1, 230400);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, camera_on_irq_cb, instance);

    return instance;
}

void camera_suite_view_camera_free(CameraSuiteViewCamera* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, UartDumpModel * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* camera_suite_view_camera_get_view(CameraSuiteViewCamera* instance) {
    furi_assert(instance);
    return instance->view;
}