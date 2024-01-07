#include "../malveke_gb_photo.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>

extern const uint8_t u8g2_font_5x7_mf[1911]; // Declare as extern

const uint8_t _I_DolphinCommon_56x48_0[] = {
    0x01, 0x00, 0xdf, 0x00, 0x00, 0x1f, 0xfe, 0x0e, 0x05, 0x3f, 0x04, 0x06, 0x78, 0x06, 0x30, 0x20,
    0xf8, 0x00, 0xc6, 0x12, 0x1c, 0x04, 0x0c, 0x0a, 0x38, 0x08, 0x08, 0x0c, 0x60, 0xc0, 0x21, 0xe0,
    0x04, 0x0a, 0x18, 0x02, 0x1b, 0x00, 0x18, 0xa3, 0x00, 0x21, 0x90, 0x01, 0x8a, 0x20, 0x02, 0x19,
    0x80, 0x18, 0x80, 0x64, 0x09, 0x20, 0x89, 0x81, 0x8c, 0x3e, 0x41, 0xe2, 0x80, 0x50, 0x00, 0x43,
    0x08, 0x01, 0x0c, 0xfc, 0x68, 0x40, 0x61, 0xc0, 0x50, 0x30, 0x00, 0x63, 0xa0, 0x7f, 0x80, 0xc4,
    0x41, 0x19, 0x07, 0xff, 0x02, 0x06, 0x18, 0x24, 0x03, 0x41, 0xf3, 0x2b, 0x10, 0x19, 0x38, 0x10,
    0x30, 0x31, 0x7f, 0xe0, 0x34, 0x08, 0x30, 0x19, 0x60, 0x80, 0x65, 0x86, 0x0a, 0x4c, 0x0c, 0x30,
    0x81, 0xb9, 0x41, 0xa0, 0x54, 0x08, 0xc7, 0xe2, 0x06, 0x8a, 0x18, 0x25, 0x02, 0x21, 0x0f, 0x19,
    0x88, 0xd8, 0x6e, 0x1b, 0x01, 0xd1, 0x1b, 0x86, 0x39, 0x66, 0x3a, 0xa4, 0x1a, 0x50, 0x06, 0x48,
    0x18, 0x18, 0xd0, 0x03, 0x01, 0x41, 0x98, 0xcc, 0x60, 0x39, 0x01, 0x49, 0x2d, 0x06, 0x03, 0x50,
    0xf8, 0x40, 0x3e, 0x02, 0xc1, 0x82, 0x86, 0xc7, 0xfe, 0x0f, 0x28, 0x2c, 0x91, 0xd2, 0x90, 0x9a,
    0x18, 0x19, 0x3e, 0x6d, 0x73, 0x12, 0x16, 0x00, 0x32, 0x49, 0x72, 0xc0, 0x7e, 0x5d, 0x44, 0xba,
    0x2c, 0x08, 0xa4, 0xc8, 0x82, 0x06, 0x17, 0xe0, 0x81, 0x90, 0x2a, 0x40, 0x61, 0xe1, 0xa2, 0x44,
    0x0c, 0x76, 0x2b, 0xe8, 0x89, 0x26, 0x43, 0x83, 0x31, 0x8c, 0x78, 0x0c, 0xb0, 0x48, 0x10, 0x1a,
    0xe0, 0x00, 0x63,
};
const uint8_t* const _I_DolphinCommon_56x48[] = {_I_DolphinCommon_56x48_0};
const Icon I_DolphinCommon_56x48 = {
    .width = 56,
    .height = 48,
    .frame_count = 1,
    .frame_rate = 0,
    .frames = _I_DolphinCommon_56x48};

struct BoilerplateScene1 {
    View* view;
    BoilerplateScene1Callback callback;
    void* context;
};

typedef struct {
    BoilerplateScene1* instance;
} BoilerplateScene1Model;

void boilerplate_scene_1_set_callback(
    BoilerplateScene1* instance,
    BoilerplateScene1Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void boilerplate_scene_1_draw(Canvas* canvas, BoilerplateScene1Model* model) {
    UNUSED(model);
    BoilerplateScene1* instance = model->instance;
    Boilerplate* app = instance->context;
    canvas_clear(canvas);
    // Prepare canvas
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, 0, 0, FRAME_WIDTH, FRAME_HEIGTH);

    if(app->show_instructions) {
        canvas_draw_icon(canvas, 71, 15, &I_DolphinCommon_56x48);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, (128 / 2), 4, AlignCenter, AlignTop, "INSTRUCTIONS");

        canvas_set_custom_u8g2_font(canvas, u8g2_font_5x7_mf); // 5x7 font, 9 lines

        canvas_draw_icon_ex(canvas, 15, 18, &I_arrow_13x6, IconRotation180); // DOWN
        canvas_draw_icon(canvas, 28, 18, &I_arrow_13x6); // UP
        canvas_draw_str_aligned(canvas, 47, 21, AlignLeft, AlignCenter, "SCROLL");
        canvas_draw_icon_ex(canvas, 33, 26, &I_arrow_13x6, IconRotation270); // PREV
        canvas_draw_icon_ex(canvas, 22, 36, &I_arrow_13x6, IconRotation90); // NEXT
        canvas_draw_str_aligned(canvas, 47, 32, AlignLeft, AlignCenter, "LEFT");
        canvas_draw_str_aligned(canvas, 47, 43, AlignLeft, AlignCenter, "RIGHT");

        canvas_set_font(canvas, FontPrimary);
        elements_button_center(canvas, "OK");
    } else {
        int count = (app->page + 1) * 0x1000;
        uint8_t status = app->scratchpad1[0x11B2 + app->page];

        storage_file_seek(app->camera_ram_sav, count, true);

        for(int y = app->pos_y; y < 14; y++) {
            for(int x = app->pos_x; x < 16; x++) {
                storage_file_read(app->camera_ram_sav, app->tile_data, sizeof(app->tile_data));
                for(int row = 0; row < 8; row++) {
                    uint8_t temp1 = app->tile_data[row * 2];
                    uint8_t temp2 = app->tile_data[row * 2 + 1];
                    for(int pixel = 7; pixel >= 0; pixel--) {
                        if(((temp1 & 1) + ((temp2 & 1) * 2)) >= 2) {
                            canvas_draw_dot(canvas, (x * 8) + pixel, (y * 8) + row);
                        }
                        temp1 >>= 1;
                        temp2 >>= 1;
                    }
                }
            }
        }

        if(app->info) {
            if(status == 0xFF) {
                canvas_draw_rbox(canvas, 100, 4, 20, 11, 4);
                canvas_invert_color(canvas);
                canvas_draw_str_aligned(canvas, 110, 10, AlignCenter, AlignCenter, "D");
                canvas_invert_color(canvas);
            }
        }
    }
}

static void boilerplate_scene_1_model_init(BoilerplateScene1Model* const model, void* context) {
    BoilerplateScene1* instance = context;
    UNUSED(model);
    UNUSED(instance);
    model->instance = context;
}

void save_image(void* context) {
    Boilerplate* app = context;
    furi_assert(app);
    NotificationApp* notifications = furi_record_open(RECORD_NOTIFICATION);

    //  Create MALVEKE dir
    if(storage_common_stat(app->storage, MALVEKE_APP_FOLDER, NULL) == FSE_NOT_EXIST) {
        storage_simply_mkdir(app->storage, MALVEKE_APP_FOLDER);
    }
    //  Create MALVEKE Photos dir
    if(storage_common_stat(app->storage, MALVEKE_APP_FOLDER_PHOTOS, NULL) == FSE_NOT_EXIST) {
        storage_simply_mkdir(app->storage, MALVEKE_APP_FOLDER_PHOTOS);
    }

    int count = (app->page + 1) * 0x1000;
    storage_file_seek(app->camera_ram_sav, count, true);
    // create file name
    FuriString* file_name = furi_string_alloc();
    get_timefilename(file_name, app->page);

    File* file = storage_file_alloc(app->storage);
    bool result =
        storage_file_open(file, furi_string_get_cstr(file_name), FSAM_WRITE, FSOM_OPEN_ALWAYS);

    if(result) {
        static char bmp[BMP_SIZE(WIDTH, HEIGHT)];
        bmp_init(bmp, WIDTH, HEIGHT);

        //  Selected Palette
        uint32_t palette[] = {
            bmp_encode(app->palette_color_hex_a),
            bmp_encode(app->palette_color_hex_b),
            bmp_encode(app->palette_color_hex_c),
            bmp_encode(app->palette_color_hex_d)};

        UNUSED(palette);

        for(int y = 0; y < 14; y++) {
            for(int x = 0; x < 16; x++) {
                storage_file_read(app->camera_ram_sav, app->tile_data, sizeof(app->tile_data));
                for(int row = 0; row < 8; row++) {
                    uint8_t temp1 = app->tile_data[row * 2];
                    uint8_t temp2 = app->tile_data[row * 2 + 1];

                    for(int pixel = 7; pixel >= 0; pixel--) {
                        bmp_set(
                            bmp,
                            (x * 8) + pixel,
                            (y * 8) + row,
                            palette[((temp1 & 1) + ((temp2 & 1) * 2))]);
                        temp1 >>= 1;
                        temp2 >>= 1;
                    }
                }
            }
        }

        storage_file_write(file, bmp, sizeof(bmp));
        storage_file_close(file);
    }

    // Closing the "file descriptor"
    storage_file_close(file);
    // Freeing up memory
    storage_file_free(file);
    notification_message(notifications, result ? &sequence_success : &sequence_error);
}
bool boilerplate_scene_1_input(InputEvent* event, void* context) {
    furi_assert(context);
    BoilerplateScene1* instance = context;
    Boilerplate* app = instance->context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                BoilerplateScene1Model * model,
                {
                    UNUSED(model);
                    instance->callback(BoilerplateCustomEventScene1Back, instance->context);
                },
                true);
            break;
        case InputKeyLeft:
            with_view_model(
                instance->view,
                BoilerplateScene1Model * model,
                {
                    UNUSED(model);
                    app->pos_y = 0;
                    app->pos_x = 0;

                    if(app->page > 0) {
                        app->page--;
                    } else {
                        app->page = 29;
                    }
                },
                true);
            break;
        case InputKeyRight:
            with_view_model(
                instance->view,
                BoilerplateScene1Model * model,
                {
                    UNUSED(model);
                    app->pos_y = 0;
                    app->pos_x = 0;
                    if(app->page < 29) {
                        app->page++;
                    } else {
                        app->page = 0;
                    }
                },
                true);
            break;
        case InputKeyUp:
            with_view_model(
                instance->view,
                BoilerplateScene1Model * model,
                {
                    UNUSED(model);
                    if(app->pos_y < 0) {
                        app->pos_y++;
                    }
                },
                true);
            break;
        case InputKeyDown:
            with_view_model(
                instance->view,
                BoilerplateScene1Model * model,
                {
                    UNUSED(model);
                    if(app->pos_y > -6) {
                        app->pos_y--;
                    }
                },
                true);
            break;
        case InputKeyOk:
            with_view_model(
                instance->view,
                BoilerplateScene1Model * model,
                {
                    UNUSED(model);
                    if(app->show_instructions) {
                        app->show_instructions = false;
                    } else {
                        save_image(app);
                    }
                },
                true);
            break;
        case InputKeyMAX:
            break;
        }
    }
    return true;
}

void boilerplate_scene_1_exit(void* context) {
    furi_assert(context);
}

void boilerplate_scene_1_enter(void* context) {
    furi_assert(context);
    BoilerplateScene1* instance = (BoilerplateScene1*)context;

    with_view_model(
        instance->view,
        BoilerplateScene1Model * model,
        { boilerplate_scene_1_model_init(model, instance); },
        true);
}

BoilerplateScene1* boilerplate_scene_1_alloc() {
    BoilerplateScene1* instance = malloc(sizeof(BoilerplateScene1));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(BoilerplateScene1Model));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)boilerplate_scene_1_draw);
    view_set_input_callback(instance->view, boilerplate_scene_1_input);
    view_set_enter_callback(instance->view, boilerplate_scene_1_enter);
    view_set_exit_callback(instance->view, boilerplate_scene_1_exit);

    with_view_model(
        instance->view,
        BoilerplateScene1Model * model,
        { boilerplate_scene_1_model_init(model, instance); },
        true);

    return instance;
}

void boilerplate_scene_1_free(BoilerplateScene1* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, BoilerplateScene1Model * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* boilerplate_scene_1_get_view(BoilerplateScene1* instance) {
    furi_assert(instance);
    return instance->view;
}
