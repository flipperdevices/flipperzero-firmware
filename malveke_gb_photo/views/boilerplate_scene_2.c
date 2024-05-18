#include "../malveke_gb_photo.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>

struct BoilerplateScene2 {
    View* view;
    BoilerplateScene2Callback callback;
    void* context;
};

typedef struct {
    bool in_progress;
    int page;
    BoilerplateScene2* instance;
} BoilerplateScene2Model;

void boilerplate_scene_2_set_callback(
    BoilerplateScene2* instance,
    BoilerplateScene2Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void draw_thumbnail(void* context, Canvas* canvas, int page) {
    BoilerplateScene2* instance = context;
    Boilerplate* app = instance->context;
    UNUSED(app);
    //  Gallery
    // for (int s=0;s< 8 * (page+1);s++) {
    // int count = ((page + 2) * 0x1000) + 0x0e00;
    int count = ((page + 2) * GB_PHOTO_SIZE) + 0x0e00;

    storage_file_seek(app->camera_ram_sav, count, true);
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            storage_file_read(app->camera_ram_sav, app->tile_data, sizeof(app->tile_data));
            for(int row = 0; row < 8; row++) {
                uint8_t temp1 = app->tile_data[row * 2];
                uint8_t temp2 = app->tile_data[row * 2 + 1];
                for(int pixel = 7; pixel >= 0; pixel--) {
                    int colorIndex = ((temp1 & 1) + ((temp2 & 1) * 2));
                    if(colorIndex >= 2) {
                        canvas_draw_dot(canvas, (x * 8) + (pixel + 47), (y * 8) + row + 17);
                    }
                    temp1 >>= 1;
                    temp2 >>= 1;
                }
            }
        }
    }
    // }
}
void boilerplate_scene_2_draw(Canvas* canvas, BoilerplateScene2Model* model) {
    UNUSED(model);
    BoilerplateScene2* instance = model->instance;
    Boilerplate* app = instance->context;
    UNUSED(app);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 21, 13, "Export All Photos");
    canvas_set_font(canvas, FontPrimary);

    char totalText[32];
    snprintf(totalText, sizeof(totalText), "%02d/29", model->page);

    draw_thumbnail(instance, canvas, model->page);
    if(!model->in_progress) {
        elements_button_center(canvas, "OK");
    } else {
        float dict_progress = (float)(model->page * 1) / (float)(29);
        int progress_width = 128 / 2;
        int position_x = (128 / 2);
        int position_y = 52;
        elements_progress_bar_with_text(
            canvas,
            position_x - (progress_width / 2),
            position_y,
            progress_width,
            dict_progress,
            totalText);
    }
}

static void boilerplate_scene_2_model_init(BoilerplateScene2Model* const model, void* context) {
    BoilerplateScene2* instance = context;
    model->instance = instance;
    model->page = 0;
    model->in_progress = false;
}
void save_all_image(void* context) {
    BoilerplateScene2* instance = context;
    Boilerplate* app = instance->context;
    UNUSED(app);
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

    for(int page = 0; page < 30; page++) {
        int count = GB_FIRST_PHOTO_OFFSET + page * GB_PHOTO_SIZE;
        storage_file_seek(app->camera_ram_sav, count, true);
        // create file name
        FuriString* file_name = furi_string_alloc();
        get_timefilename(file_name, page);

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

            with_view_model(
                instance->view,
                BoilerplateScene2Model * model,
                {
                    // UNUSED(model);
                    model->page = page;
                },
                true);
            furi_delay_ms(120);
        }

        // Closing the "file descriptor"
        storage_file_close(file);
        // Freeing up memory
        storage_file_free(file);
    }
    notification_message(notifications, &sequence_success);
    with_view_model(
        instance->view, BoilerplateScene2Model * model, { model->in_progress = false; }, true);
}
bool boilerplate_scene_2_input(InputEvent* event, void* context) {
    furi_assert(context);
    BoilerplateScene2* instance = context;
    // Boilerplate *app = instance->context;

    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyOk:
            with_view_model(
                instance->view,
                BoilerplateScene2Model * model,
                { model->in_progress = true; },
                true);

            save_all_image(instance);
            break;
        case InputKeyBack:
            instance->callback(BoilerplateCustomEventScene2Back, instance->context);
            break;
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyMAX:
            break;
        }
    }

    return true;
}

void boilerplate_scene_2_exit(void* context) {
    furi_assert(context);
    Boilerplate* app = context;
    UNUSED(app);
    // boilerplate_stop_all_sound(app);
    // boilerplate_led_reset(app);
}

BoilerplateScene2* boilerplate_scene_2_alloc() {
    BoilerplateScene2* instance = malloc(sizeof(BoilerplateScene2));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(BoilerplateScene2Model));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)boilerplate_scene_2_draw);
    view_set_input_callback(instance->view, boilerplate_scene_2_input);
    view_set_exit_callback(instance->view, boilerplate_scene_2_exit);

    with_view_model(
        instance->view,
        BoilerplateScene2Model * model,
        { boilerplate_scene_2_model_init(model, instance); },
        true);

    return instance;
}

void boilerplate_scene_2_free(BoilerplateScene2* instance) {
    furi_assert(instance);

    view_free(instance->view);
    free(instance);
}

View* boilerplate_scene_2_get_view(BoilerplateScene2* instance) {
    furi_assert(instance);

    return instance->view;
}
