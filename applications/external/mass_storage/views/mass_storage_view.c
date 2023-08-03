#include "mass_storage_view.h"
#include <gui/elements.h>
#include <mass_storage_icons.h>

struct MassStorage {
    View* view;
};

typedef struct {
    FuriString* file_name;
} MassStorageModel;

static void mass_storage_draw_callback(Canvas* canvas, void* _model) {
    MassStorageModel* model = _model;

    canvas_draw_icon(canvas, 8, 14, &I_Drive_112x35);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas, canvas_width(canvas) / 2, 0, AlignCenter, AlignTop, "USB Mass Storage");

    elements_string_fit_width(canvas, model->file_name, 87 - 2);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 16, 27, "Disc image:");
    canvas_draw_str(canvas, 16, 40, furi_string_get_cstr(model->file_name));
}

MassStorage* mass_storage_alloc() {
    MassStorage* mass_storage = malloc(sizeof(MassStorage));

    mass_storage->view = view_alloc();
    view_allocate_model(mass_storage->view, ViewModelTypeLocking, sizeof(MassStorageModel));
    with_view_model(
        mass_storage->view,
        MassStorageModel * model,
        { model->file_name = furi_string_alloc(); },
        false);
    view_set_context(mass_storage->view, mass_storage);
    view_set_draw_callback(mass_storage->view, mass_storage_draw_callback);

    return mass_storage;
}

void mass_storage_free(MassStorage* mass_storage) {
    furi_assert(mass_storage);
    with_view_model(
        mass_storage->view,
        MassStorageModel * model,
        { furi_string_free(model->file_name); },
        false);
    view_free(mass_storage->view);
    free(mass_storage);
}

View* mass_storage_get_view(MassStorage* mass_storage) {
    furi_assert(mass_storage);
    return mass_storage->view;
}

void mass_storage_set_file_name(MassStorage* mass_storage, FuriString* name) {
    furi_assert(name);
    with_view_model(
        mass_storage->view,
        MassStorageModel * model,
        { furi_string_set(model->file_name, name); },
        true);
}
