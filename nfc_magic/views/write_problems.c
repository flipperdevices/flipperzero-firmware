#include "write_problems.h"

#include <gui/elements.h>
#include "nfc_magic_icons.h"

struct WriteProblems {
    View* view;
    WriteProblemsCallback callback;
    void* context;
};

typedef struct {
    uint8_t problem_index;
    uint8_t problems_total;
    FuriString* content;
} WriteProblemsViewModel;

static void write_problems_view_draw_callback(Canvas* canvas, void* _model) {
    WriteProblemsViewModel* model = _model;
    FuriString* header = furi_string_alloc();
    canvas_clear(canvas);

    // Header
    if(model->problems_total > 1) {
        furi_string_printf(
            header, "Warnings: %d of %d\n", model->problem_index + 1, model->problems_total);
    } else {
        furi_string_printf(header, "Warning!");
    }

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, furi_string_get_cstr(header));

    // Warning message
    canvas_set_font(canvas, FontSecondary);
    elements_text_box(
        canvas, 1, 13, 76, 42, AlignLeft, AlignTop, furi_string_get_cstr(model->content), false);

    // Butttons
    if(model->problem_index == model->problems_total - 1) {
        elements_button_center(canvas, "Skip");
        elements_button_left(canvas, "Retry");
    } else {
        elements_button_center(canvas, "Next");
        elements_button_left(canvas, "Back");
    }

    // Dolphin
    canvas_draw_icon(canvas, 83, 22, &I_WarningDolphinFlip_45x42);

    furi_string_free(header);
}

static bool write_problems_input_callback(InputEvent* event, void* context) {
    WriteProblems* instance = context;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyLeft) {
            instance->callback(WriteProblemsEventLeftPressed, instance->context);
            return true;
        } else if(event->key == InputKeyOk) {
            instance->callback(WriteProblemsEventCenterPressed, instance->context);
            return true;
        }
    }
    return false;
}

WriteProblems* write_problems_alloc() {
    WriteProblems* instance = malloc(sizeof(WriteProblems));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(WriteProblemsViewModel));
    view_set_draw_callback(instance->view, write_problems_view_draw_callback);
    view_set_input_callback(instance->view, write_problems_input_callback);
    view_set_context(instance->view, instance);
    with_view_model(
        instance->view,
        WriteProblemsViewModel * model,
        {
            model->content = furi_string_alloc();
            model->problem_index = 0;
            model->problems_total = 0;
        },
        false);

    return instance;
}

void write_problems_free(WriteProblems* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        WriteProblemsViewModel * model,
        { furi_string_free(model->content); },
        false);

    view_free(instance->view);
    free(instance);
}

void write_problems_reset(WriteProblems* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        WriteProblemsViewModel * model,
        {
            model->problem_index = 0;
            model->problems_total = 0;
            furi_string_reset(model->content);
        },
        true);
}

View* write_problems_get_view(WriteProblems* instance) {
    furi_assert(instance);

    return instance->view;
}

void write_problems_set_callback(
    WriteProblems* instance,
    WriteProblemsCallback callback,
    void* context) {
    furi_assert(instance);
    instance->callback = callback;
    instance->context = context;
}

void write_problems_set_content(WriteProblems* instance, const char* content) {
    furi_assert(instance);
    furi_assert(content);

    with_view_model(
        instance->view,
        WriteProblemsViewModel * model,
        { furi_string_set(model->content, content); },
        true);
}

void write_problems_set_problems_total(WriteProblems* instance, uint8_t problems_total) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        WriteProblemsViewModel * model,
        { model->problems_total = problems_total; },
        true);
}

void write_problems_set_problem_index(WriteProblems* instance, uint8_t problem_index) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        WriteProblemsViewModel * model,
        { model->problem_index = problem_index; },
        true);
}