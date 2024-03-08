#include "../camera_suite.h"
#include "camera_suite_view_camera.h"

#include "../helpers/camera_suite_haptic.h"
#include "../helpers/camera_suite_led.h"
#include "../helpers/camera_suite_speaker.h"
#include "../helpers/camera_suite_custom_event.h"
// #include "../helpers/camera_suite_uart.h"

#define BITMAP_HEADER_LENGTH 62
#define FRAME_BIT_DEPTH 1
#define HEADER_LENGTH 3 // 'Y', ':', and row identifier
#define LAST_ROW_INDEX 1008
#define ROW_BUFFER_LENGTH 16
#define RING_BUFFER_LENGTH 19
#define FRAME_BUFFER_LENGTH 1024

static const unsigned char bitmap_header[BITMAP_HEADER_LENGTH] = {
    0x42, 0x4D, 0x3E, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00,
    0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00};

static void draw_pixel_by_orientation(Canvas* canvas, uint8_t x, uint8_t y, uint8_t orientation) {
    furi_assert(canvas);
    furi_assert(x);
    furi_assert(y);
    furi_assert(orientation);

    switch(orientation) {
    default:
    case 0: { // Camera rotated 0 degrees (right side up, default)
        canvas_draw_dot(canvas, x, y);
        break;
    }
    case 1: { // Camera rotated 90 degrees

        canvas_draw_dot(canvas, y, FLIPPER_SCREEN_WIDTH - 1 - x);
        break;
    }
    case 2: { // Camera rotated 180 degrees (upside down)
        canvas_draw_dot(canvas, FLIPPER_SCREEN_WIDTH - 1 - x, FLIPPER_SCREEN_HEIGHT - 1 - y);
        break;
    }
    case 3: { // Camera rotated 270 degrees
        canvas_draw_dot(canvas, FLIPPER_SCREEN_HEIGHT - 1 - y, x);
        break;
    }
    }
}

static void camera_suite_view_camera_draw(Canvas* canvas, void* uart_dump_model) {
    furi_assert(canvas);
    furi_assert(uart_dump_model);

    UartDumpModel* model = uart_dump_model;

    // Clear the screen.
    canvas_set_color(canvas, ColorBlack);

    // Draw the frame.
    canvas_draw_frame(canvas, 0, 0, FLIPPER_SCREEN_WIDTH, FLIPPER_SCREEN_HEIGHT);

    for(size_t p = 0; p < FRAME_BUFFER_LENGTH; ++p) {
        uint8_t x = p % ROW_BUFFER_LENGTH; // 0 .. 15
        uint8_t y = p / ROW_BUFFER_LENGTH; // 0 .. 63

        for(uint8_t i = 0; i < 8; ++i) {
            if((model->pixels[p] & (1 << (7 - i))) != 0) {
                draw_pixel_by_orientation(canvas, (x * 8) + i, y, model->orientation);
            }
        }
    }

    // Draw the pinout guide if the camera is not initialized.
    if(!model->is_initialized) {
        // Clear the screen.
        canvas_clear(canvas);

        // Draw the ESP32-CAM module.
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 47, 50, "ESP32");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 52, 58, "CAM");
        canvas_draw_dot(canvas, 84, 3);
        canvas_draw_box(canvas, 50, 35, 23, 7);
        canvas_draw_circle(canvas, 42, 12, 1);
        canvas_draw_circle(canvas, 42, 16, 1);
        canvas_draw_circle(canvas, 42, 20, 1);
        canvas_draw_circle(canvas, 42, 24, 1);
        canvas_draw_circle(canvas, 42, 28, 1);
        canvas_draw_circle(canvas, 42, 32, 1);
        canvas_draw_circle(canvas, 42, 36, 1);
        canvas_draw_circle(canvas, 42, 8, 1);
        canvas_draw_circle(canvas, 59, 15, 1);
        canvas_draw_circle(canvas, 61, 17, 5);
        canvas_draw_circle(canvas, 61, 17, 9);
        canvas_draw_circle(canvas, 80, 12, 1);
        canvas_draw_circle(canvas, 80, 16, 1);
        canvas_draw_circle(canvas, 80, 20, 1);
        canvas_draw_circle(canvas, 80, 24, 1);
        canvas_draw_circle(canvas, 80, 28, 1);
        canvas_draw_circle(canvas, 80, 32, 1);
        canvas_draw_circle(canvas, 80, 36, 1);
        canvas_draw_circle(canvas, 80, 42, 1);
        canvas_draw_circle(canvas, 80, 8, 1);
        canvas_draw_line(canvas, 38, 4, 38, 58);
        canvas_draw_line(canvas, 39, 3, 83, 3);
        canvas_draw_line(canvas, 40, 2, 84, 2);
        canvas_draw_line(canvas, 48, 4, 74, 4);
        canvas_draw_line(canvas, 48, 5, 48, 26);
        canvas_draw_line(canvas, 55, 27, 49, 27);
        canvas_draw_line(canvas, 56, 25, 56, 36);
        canvas_draw_line(canvas, 64, 21, 63, 21);
        canvas_draw_line(canvas, 65, 15, 65, 17);
        canvas_draw_line(canvas, 66, 15, 64, 18);
        canvas_draw_line(canvas, 66, 16, 64, 19);
        canvas_draw_line(canvas, 66, 18, 60, 21);
        canvas_draw_line(canvas, 66, 19, 61, 21);
        canvas_draw_line(canvas, 66, 25, 66, 36);
        canvas_draw_line(canvas, 73, 27, 67, 27);
        canvas_draw_line(canvas, 74, 5, 74, 26);
        canvas_draw_line(canvas, 75, 4, 75, 25);
        canvas_draw_line(canvas, 83, 59, 39, 59);
        canvas_draw_line(canvas, 84, 4, 84, 58);
        canvas_draw_line(canvas, 85, 2, 85, 57);
        canvas_draw_frame(canvas, 78, 40, 5, 5);

        // Draw the pinout lines.
        canvas_draw_line(canvas, 39, 8, 21, 8);
        canvas_draw_line(canvas, 87, 24, 83, 24);
        canvas_draw_line(canvas, 87, 32, 83, 32);
        canvas_draw_line(canvas, 88, 23, 88, 13);
        canvas_draw_line(canvas, 88, 33, 88, 43);
        canvas_draw_line(canvas, 89, 12, 126, 12);
        canvas_draw_line(canvas, 126, 28, 83, 28);
        canvas_draw_line(canvas, 126, 44, 89, 44);

        // Draw the pinout labels.
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 91, 11, "VCC-3V");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 91, 27, "U0R-TX");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 91, 43, "U0T-RX");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 2, 12, "GND");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 12, 21, "-GND");

        // Draw the "Please Connect Module!" text.
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 2, 40, "Please");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 2, 49, "Connect");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 2, 58, "Module!");

        // Draw the "Back" text and button logo.
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 92, 57, "Back");
        canvas_draw_line(canvas, 116, 49, 116, 53);
        canvas_draw_line(canvas, 115, 50, 115, 52);
        canvas_draw_dot(canvas, 114, 51);
        canvas_draw_line(canvas, 117, 51, 121, 51);
        canvas_draw_line(canvas, 122, 52, 123, 53);
        canvas_draw_line(canvas, 123, 54, 122, 55);
        canvas_draw_line(canvas, 121, 56, 117, 56);
    }
}

static void save_image_to_flipper_sd_card(void* uart_dump_model) {
    furi_assert(uart_dump_model);

    UartDumpModel* model = uart_dump_model;

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
    FuriHalRtcDateTime datetime = {};
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

    if(!model->is_inverted) {
        for(size_t i = 0; i < FRAME_BUFFER_LENGTH; ++i) {
            model->pixels[i] = ~model->pixels[i];
        }
    }

    // If the file was opened successfully, write the bitmap header and the
    // image data.
    if(result) {
        // Write BMP Header
        storage_file_write(file, bitmap_header, BITMAP_HEADER_LENGTH);

        // @todo - Add a function for saving the image directly from the
        // ESP32-CAM to the Flipper Zero SD card.

        // Write locally to the Flipper Zero SD card in the DCIM folder.
        int8_t row_buffer[ROW_BUFFER_LENGTH];

        // @todo - Save image based on orientation.
        for(size_t i = 64; i > 0; --i) {
            for(size_t j = 0; j < ROW_BUFFER_LENGTH; ++j) {
                row_buffer[j] = model->pixels[((i - 1) * ROW_BUFFER_LENGTH) + j];
            }
            storage_file_write(file, row_buffer, ROW_BUFFER_LENGTH);
        }
    }

    // Close the file.
    storage_file_close(file);

    // Free up memory.
    storage_file_free(file);
}

static void camera_suite_view_camera_model_init(UartDumpModel* model, CameraSuite* app_instance) {
    furi_assert(model);
    furi_assert(app_instance);

    model->is_dithering_enabled = true;
    model->is_inverted = false;
    uint32_t orientation = app_instance->orientation;
    model->orientation = orientation;
    for(size_t i = 0; i < FRAME_BUFFER_LENGTH; i++) {
        model->pixels[i] = 0;
    }
}

static bool camera_suite_view_camera_input(InputEvent* input_event, void* camera_view_instance) {
    furi_assert(camera_view_instance);
    furi_assert(input_event);

    CameraSuiteViewCamera* instance = camera_view_instance;
    CameraSuite* app_instance = instance->context;
    uint8_t data[1];

    if(input_event->type == InputTypeRelease) {
        if(input_event->key) {
            // Stop all sounds, reset the LED.
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
        }
    } else if(input_event->type == InputTypePress) {
        switch(input_event->key) {
        case InputKeyBack: {
            with_view_model(
                instance->view,
                UartDumpModel * model,
                {
                    UNUSED(model);

                    // Stop camera stream.
                    data[0] = 's';
                    furi_hal_serial_tx(app_instance->serial_handle, data, 1);
                    furi_delay_ms(50);

                    // Go back to the main menu.
                    instance->callback(CameraSuiteCustomEventSceneCameraBack, instance->context);
                },
                true);
            break;
        }
        case InputKeyLeft: {
            with_view_model(
                instance->view,
                UartDumpModel * model,
                {
                    // Play sound.
                    camera_suite_play_happy_bump(instance->context);
                    camera_suite_play_input_sound(instance->context);
                    camera_suite_led_set_rgb(instance->context, 0, 0, 255);

                    if(model->is_inverted) {
                        // Camera: Set invert to false on the ESP32-CAM.
                        data[0] = 'i';
                        furi_hal_serial_tx(app_instance->serial_handle, data, 1);
                        furi_delay_ms(50);

                        model->is_inverted = false;
                    } else {
                        // Camera: Set invert to true on the ESP32-CAM.
                        data[0] = 'I';
                        furi_hal_serial_tx(app_instance->serial_handle, data, 1);
                        furi_delay_ms(50);

                        model->is_inverted = true;
                    }

                    instance->callback(CameraSuiteCustomEventSceneCameraLeft, instance->context);
                },
                true);
            break;
        }
        case InputKeyRight: {
            with_view_model(
                instance->view,
                UartDumpModel * model,
                {
                    // Play sound.
                    camera_suite_play_happy_bump(instance->context);
                    camera_suite_play_input_sound(instance->context);
                    camera_suite_led_set_rgb(instance->context, 0, 0, 255);

                    if(model->is_dithering_enabled) {
                        // Camera: Disable dithering.
                        data[0] = 'd';
                        furi_hal_serial_tx(app_instance->serial_handle, data, 1);
                        furi_delay_ms(50);

                        model->is_dithering_enabled = false;
                    } else {
                        // Camera: Enable dithering.
                        data[0] = 'D';
                        furi_hal_serial_tx(app_instance->serial_handle, data, 1);
                        furi_delay_ms(50);

                        model->is_dithering_enabled = true;
                    }

                    instance->callback(CameraSuiteCustomEventSceneCameraRight, instance->context);
                },
                true);
            break;
        }
        case InputKeyUp: {
            with_view_model(
                instance->view,
                UartDumpModel * model,
                {
                    UNUSED(model);

                    // Play sound.
                    camera_suite_play_happy_bump(instance->context);
                    camera_suite_play_input_sound(instance->context);
                    camera_suite_led_set_rgb(instance->context, 0, 0, 255);

                    // Camera: Increase contrast.
                    data[0] = 'C';
                    furi_hal_serial_tx(app_instance->serial_handle, data, 1);
                    furi_delay_ms(50);

                    instance->callback(CameraSuiteCustomEventSceneCameraUp, instance->context);
                },
                true);
            break;
        }
        case InputKeyDown: {
            with_view_model(
                instance->view,
                UartDumpModel * model,
                {
                    UNUSED(model);

                    // Play sound.
                    camera_suite_play_happy_bump(instance->context);
                    camera_suite_play_input_sound(instance->context);
                    camera_suite_led_set_rgb(instance->context, 0, 0, 255);

                    // Camera: Reduce contrast.
                    data[0] = 'c';
                    furi_hal_serial_tx(app_instance->serial_handle, data, 1);
                    furi_delay_ms(50);

                    instance->callback(CameraSuiteCustomEventSceneCameraDown, instance->context);
                },
                true);
            break;
        }
        case InputKeyOk: {
            with_view_model(
                instance->view,
                UartDumpModel * model,
                {
                    // Play sound.
                    camera_suite_play_long_bump(instance->context);
                    camera_suite_play_input_sound(instance->context);
                    camera_suite_led_set_rgb(instance->context, 0, 0, 255);

                    // @todo - Save picture directly to ESP32-CAM.
                    // data[0] = 'P';
                    // furi_hal_serial_tx(instance->camera_serial_handle, data, 1);

                    // Save currently displayed image to the Flipper Zero SD card.
                    save_image_to_flipper_sd_card(model);

                    instance->callback(CameraSuiteCustomEventSceneCameraOk, instance->context);
                },
                true);
            break;
        }
        case InputKeyMAX:
        default: {
            break;
        }
        }
    }

    return false;
}

static void camera_suite_view_camera_exit(void* camera_view_instance) {
    UNUSED(camera_view_instance);
}

static void camera_suite_view_camera_enter(void* camera_view_instance) {
    furi_assert(camera_view_instance);

    CameraSuiteViewCamera* instance = camera_view_instance;
    CameraSuite* app_instance = instance->context;

    uint8_t data[1];

    // Start serial stream to Flipper Zero.
    data[0] = 'S';
    furi_hal_serial_tx(app_instance->serial_handle, data, 1);
    furi_delay_ms(50);

    // Get/set dither type.
    uint8_t dither_type = app_instance->dither;
    furi_hal_serial_tx(app_instance->serial_handle, &dither_type, 1);
    furi_delay_ms(50);

    // Make sure the camera is not inverted.
    data[0] = 'i';
    furi_hal_serial_tx(app_instance->serial_handle, data, 1);
    furi_delay_ms(50);

    // Toggle flash on or off based on the current state. If the user has this
    // on the flash will stay on the entire time the user is in the camera view.
    data[0] = app_instance->flash ? 'F' : 'f';
    furi_hal_serial_tx(app_instance->serial_handle, data, 1);
    furi_delay_ms(50);

    with_view_model(
        instance->view,
        UartDumpModel * model,
        { camera_suite_view_camera_model_init(model, app_instance); },
        true);
}

static void camera_callback(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxEvent event,
    void* camera_view_instance) {
    furi_assert(handle);
    furi_assert(camera_view_instance);

    CameraSuiteViewCamera* instance = camera_view_instance;
    CameraSuite* app_instance = instance->context;

    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        furi_stream_buffer_send(app_instance->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(app_instance->worker_thread), WorkerEventRx);
    }
}

static void process_ringbuffer(UartDumpModel* model, uint8_t const byte) {
    furi_assert(model);
    furi_assert(byte);

    // The first HEADER_LENGTH bytes are reserved for header information.
    if(model->ringbuffer_index < HEADER_LENGTH) {
        // Validate the start of row characters 'Y' and ':'.
        if(model->ringbuffer_index == 0 && byte != 'Y') {
            // Incorrect start of frame; reset.
            return;
        }
        if(model->ringbuffer_index == 1 && byte != ':') {
            // Incorrect start of frame; reset.
            model->ringbuffer_index = 0;
            return;
        }
        if(model->ringbuffer_index == 2) {
            // Assign the third byte as the row identifier.
            model->row_identifier = byte;
        }
        model->ringbuffer_index++; // Increment index for the next byte.
        return;
    }

    // Store pixel value directly after the header.
    model->row_ringbuffer[model->ringbuffer_index - HEADER_LENGTH] = byte;
    model->ringbuffer_index++; // Increment index for the next byte.

    // Check whether the ring buffer is filled.
    if(model->ringbuffer_index >= RING_BUFFER_LENGTH) {
        model->ringbuffer_index = 0; // Reset the ring buffer index.
        model->is_initialized = true; // Set the connection as successfully established.

        // Compute the starting index for the row in the pixel buffer.
        size_t row_start_index = model->row_identifier * ROW_BUFFER_LENGTH;

        // Ensure the row start index is within the valid range.
        if(row_start_index > LAST_ROW_INDEX) {
            row_start_index = 0; // Reset to a safe value in case of an overflow.
        }

        // Flush the contents of the ring buffer to the pixel buffer.
        for(size_t i = 0; i < ROW_BUFFER_LENGTH; ++i) {
            model->pixels[row_start_index + i] = model->row_ringbuffer[i];
        }
    }
}

static int32_t camera_suite_camera_worker(void* camera_view_instance) {
    furi_assert(camera_view_instance);

    CameraSuiteViewCamera* instance = camera_view_instance;
    CameraSuite* app_instance = instance->context;

    while(1) {
        // Wait for any event on the worker thread.
        uint32_t events =
            furi_thread_flags_wait(WORKER_EVENTS_MASK, FuriFlagWaitAny, FuriWaitForever);

        // Check if an error occurred.
        furi_check((events & FuriFlagError) == 0);

        // Check if the thread should stop.
        if(events & WorkerEventStop) {
            break;
        } else if(events & WorkerEventRx) {
            size_t length = 0;
            // Read all available data from the stream buffer.
            do {
                // Read up to 64 bytes from the stream buffer.
                size_t buffer_size = 64;
                // Allocate a buffer for the data.
                uint8_t data[buffer_size];
                // Read the data from the stream buffer.
                length = furi_stream_buffer_receive(app_instance->rx_stream, data, buffer_size, 0);
                if(length > 0) {
                    with_view_model(
                        instance->view,
                        UartDumpModel * model,
                        {
                            // Process the data.
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

    // Allocate the view object
    instance->view = view_alloc();

    // Allocate model
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(UartDumpModel));

    // Set context for the view
    view_set_context(instance->view, instance);

    // Set draw callback
    view_set_draw_callback(instance->view, (ViewDrawCallback)camera_suite_view_camera_draw);

    // Set input callback
    view_set_input_callback(instance->view, camera_suite_view_camera_input);

    // Set enter callback
    view_set_enter_callback(instance->view, camera_suite_view_camera_enter);

    // Set exit callback
    view_set_exit_callback(instance->view, camera_suite_view_camera_exit);

    // Allocate the UART worker thread for the camera.
    // CameraSuite* app_instance = instance->context;
    // camera_suite_uart_alloc(app_instance, camera_callback);

    with_view_model(
        instance->view,
        UartDumpModel * model,
        { camera_suite_view_camera_model_init(model, instance); },
        true);

    return instance;
}

void camera_suite_view_camera_free(CameraSuiteViewCamera* instance) {
    furi_assert(instance);

    CameraSuite* app_instance = instance->context;

    // Free the UART worker thread.
    // camera_suite_uart_free(app_instance);

    with_view_model(
        instance->view, UartDumpModel * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* camera_suite_view_camera_get_view(CameraSuiteViewCamera* instance) {
    furi_assert(instance);
    return instance->view;
}

void camera_suite_view_camera_set_callback(
    CameraSuiteViewCamera* camera_view_instance,
    CameraSuiteViewCameraCallback callback,
    void* context) {
    furi_assert(camera_view_instance);
    furi_assert(callback);
    camera_view_instance->callback = callback;
    camera_view_instance->context = context;
}
