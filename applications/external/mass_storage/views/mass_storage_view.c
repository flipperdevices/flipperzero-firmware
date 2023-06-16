#include "mass_storage_view.h"
#include <gui/elements.h>
#include <assets_icons.h>

struct MassStorage {
    View* view;
};

typedef struct {
    char* file_name;
} MassStorageModel;

static void mass_storage_draw_callback(Canvas* canvas, void* _model) {
    MassStorageModel* model = _model;

    FuriString* disp_str = furi_string_alloc_set(model->file_name);
    elements_string_fit_width(canvas, disp_str, 128 - 2);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 8, furi_string_get_cstr(disp_str));
    furi_string_reset(disp_str);

    canvas_draw_icon(canvas, 40, 20, &I_UsbTree_48x22);

    furi_string_free(disp_str);
}

MassStorage* mass_storage_alloc() {
    MassStorage* mass_storage = malloc(sizeof(MassStorage));

    mass_storage->view = view_alloc();
    view_allocate_model(mass_storage->view, ViewModelTypeLocking, sizeof(MassStorageModel));
    view_set_context(mass_storage->view, mass_storage);
    view_set_draw_callback(mass_storage->view, mass_storage_draw_callback);

    return mass_storage;
}

void mass_storage_free(MassStorage* mass_storage) {
    furi_assert(mass_storage);
    view_free(mass_storage->view);
    free(mass_storage);
}

View* mass_storage_get_view(MassStorage* mass_storage) {
    furi_assert(mass_storage);
    return mass_storage->view;
}

void mass_storage_set_file_name(MassStorage* mass_storage, char* name) {
    furi_assert(name);
    with_view_model(
        mass_storage->view, MassStorageModel * model, { model->file_name = name; }, true);
}
