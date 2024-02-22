#include "progress.h"

#include <gui/canvas.h>
#include <gui/elements.h>

#include "video_game_module_tool_icons.h"

struct Progress {
    View* view;
};

typedef struct {
    FuriString* text;
    uint8_t progress;
} ProgressModel;

static void progress_draw_callback(Canvas* canvas, void* _model) {
    ProgressModel* model = _model;

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "INSTALLING");
    canvas_draw_icon(canvas, 34, 11, &I_Flashing_module_70x30);

    elements_progress_bar_with_text(
        canvas, 22, 45, 84, model->progress / 100.f, furi_string_get_cstr(model->text));
}

Progress* progress_alloc() {
    Progress* instance = malloc(sizeof(Progress));
    instance->view = view_alloc();

    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(ProgressModel));
    view_set_draw_callback(instance->view, progress_draw_callback);

    with_view_model(
        instance->view,
        ProgressModel * model,
        {
            model->progress = 0;
            model->text = furi_string_alloc_printf("0%%");
        },
        true);

    return instance;
}

void progress_free(Progress* instance) {
    with_view_model(
        instance->view, ProgressModel * model, { furi_string_free(model->text); }, false);

    view_free(instance->view);
    free(instance);
}

View* progress_get_view(Progress* instance) {
    return instance->view;
}

void progress_set_value(Progress* instance, uint8_t value) {
    bool update = false;
    with_view_model(
        instance->view,
        ProgressModel * model,
        {
            update = model->progress != value;
            if(update) {
                furi_string_printf(model->text, "%u%%", value);
                model->progress = value;
            }
        },
        update);
}

void progress_reset(Progress* instance) {
    progress_set_value(instance, 0);
}
