#include "mass_storage_view.h"
#include <gui/elements.h>
#include <mass_storage_icons.h>

struct MassStorage {
    View* view;
};

typedef struct {
    FuriString *file_name, *status_string;
    uint32_t read_speed, write_speed;
    uint32_t bytes_read, bytes_written;
    uint32_t update_time;
} MassStorageModel;

static void append_suffixed_byte_count(FuriString* string, uint32_t count) {
    if(count < 1024) {
        furi_string_cat_printf(string, "%luB", count);
    } else if(count < 1024 * 1024) {
        furi_string_cat_printf(string, "%luK", count / 1024);
    } else if(count < 1024 * 1024 * 1024) {
        furi_string_cat_printf(string, "%.3fM", (double)count / (1024 * 1024));
    } else {
        furi_string_cat_printf(string, "%.3fG", (double)count / (1024 * 1024 * 1024));
    }
}

static void mass_storage_draw_callback(Canvas* canvas, void* _model) {
    MassStorageModel* model = _model;

    canvas_draw_icon(canvas, 8, 14, &I_Drive_112x35);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas, canvas_width(canvas) / 2, 0, AlignCenter, AlignTop, "USB Mass Storage");

    canvas_set_font(canvas, FontSecondary);
    elements_string_fit_width(canvas, model->file_name, 89 - 2);
    canvas_draw_str_aligned(
        canvas, 50, 23, AlignCenter, AlignBottom, furi_string_get_cstr(model->file_name));

    furi_string_set_str(model->status_string, "R:");
    append_suffixed_byte_count(model->status_string, model->bytes_read);
    if(model->read_speed) {
        furi_string_cat_str(model->status_string, "; ");
        append_suffixed_byte_count(model->status_string, model->read_speed);
        furi_string_cat_str(model->status_string, "ps");
    }
    canvas_draw_str(canvas, 12, 34, furi_string_get_cstr(model->status_string));

    furi_string_set_str(model->status_string, "W:");
    append_suffixed_byte_count(model->status_string, model->bytes_written);
    if(model->write_speed) {
        furi_string_cat_str(model->status_string, "; ");
        append_suffixed_byte_count(model->status_string, model->write_speed);
        furi_string_cat_str(model->status_string, "ps");
    }
    canvas_draw_str(canvas, 12, 44, furi_string_get_cstr(model->status_string));
}

MassStorage* mass_storage_alloc() {
    MassStorage* mass_storage = malloc(sizeof(MassStorage));

    mass_storage->view = view_alloc();
    view_allocate_model(mass_storage->view, ViewModelTypeLocking, sizeof(MassStorageModel));
    with_view_model(
        mass_storage->view,
        MassStorageModel * model,
        {
            model->file_name = furi_string_alloc();
            model->status_string = furi_string_alloc();
        },
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
        {
            furi_string_free(model->file_name);
            furi_string_free(model->status_string);
        },
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

void mass_storage_set_stats(MassStorage* mass_storage, uint32_t read, uint32_t written) {
    with_view_model(
        mass_storage->view,
        MassStorageModel * model,
        {
            uint32_t now = furi_get_tick();
            model->read_speed = (read - model->bytes_read) * 1000 / (now - model->update_time);
            model->write_speed =
                (written - model->bytes_written) * 1000 / (now - model->update_time);
            model->bytes_read = read;
            model->bytes_written = written;
            model->update_time = now;
        },
        true);
}
