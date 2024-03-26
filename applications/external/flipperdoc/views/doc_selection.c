#include "doc_selection.h"
#include "doc_view_common.h"

#include <doc_icons.h>

struct DocSelection {
    View* view;
    DocSelectionCallback callback;
    void* ctx;
};

typedef struct {
    const char* title;
    FuriString* text;
    const char* footer;

    uint8_t category;

    //Internal
    uint8_t size;
    uint8_t position;
    uint8_t window_position;
} DocSelectionViewModel;

static void doc_selection_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    DocSelectionViewModel* vm = ctx;

    canvas_draw_icon(canvas, 0, 0, &I_doc_smallscreen_light);

    // Selection Arrow
    uint8_t selection_index = vm->position - vm->window_position;
    if(selection_index == 0) {
        canvas_draw_icon(canvas, 123, 16, &I_doc_button_left_small);
    } else if(selection_index == 1) {
        canvas_draw_icon(canvas, 123, 25, &I_doc_button_left_small);
    } else {
        canvas_draw_icon(canvas, 123, 34, &I_doc_button_left_small);
    }

    // Title
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 85, 11, AlignCenter, AlignBottom, vm->title);
    canvas_set_font(canvas, FontSecondary);

    // Text
    doc_draw_text(canvas, vm->text, 44, 121, 22, 9, vm->window_position, 3);

    // Footer
    canvas_draw_str_aligned(canvas, 87, 54, AlignCenter, AlignBottom, vm->footer);
}

static bool doc_selection_input_callback(InputEvent* event, void* ctx) {
    furi_assert(ctx);
    DocSelection* instance = ctx;
    bool consumed = false;

    if((event->type == InputTypeShort) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyUp:
            with_view_model(
                instance->view,
                DocSelectionViewModel* model,
                {
                    if(model->position > 0) {
                        model->position--;

                        if(model->position == model->window_position && model->window_position > 0) {
                            model->window_position--;
                        }
                    } else {
                        model->position = model->size - 1;

                        if(model->position > 2) {
                            model->window_position = model->position - 2;
                        }
                    }
                },
                true);
            consumed = true;
            break;
        case InputKeyDown:
            with_view_model(
                instance->view,
                DocSelectionViewModel* model,
                {
                    if(model->position < model->size - 1) {
                        model->position++;

                        if(model->position - model->window_position > 1 && model->window_position < model->size - 3) {
                            model->window_position++;
                        }
                    } else {
                        model->position = 0;
                        model->window_position = 0;
                    }
                },
                true);
            consumed = true;
            break;
        case InputKeyOk:
            with_view_model(
                instance->view,
                DocSelectionViewModel* model,
                {
                    if(instance->callback) {
                        instance->callback(instance->ctx, model->position);
                    }
                },
                false);
            break;
        default:
            break;
        }
    }

    return consumed;
}

//? Basic Functions
View* doc_selection_get_view(DocSelection* instance) {
    furi_assert(instance);
    return instance->view;
}

void doc_selection_set_callback(DocSelection* instance, DocSelectionCallback callback, void* ctx) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->ctx = ctx;
}

DocSelection* doc_selection_alloc() {
    DocSelection* instance = malloc(sizeof(DocSelection));

    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(DocSelectionViewModel));
    view_set_draw_callback(instance->view, doc_selection_draw_callback);
    view_set_input_callback(instance->view, doc_selection_input_callback);
    view_set_context(instance->view, instance);

    with_view_model(
        instance->view,
        DocSelectionViewModel* vm,
        {
            vm->text = furi_string_alloc();
        }, false);

    return instance;
}

void doc_selection_free(DocSelection* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        DocSelectionViewModel* vm,
        {
            furi_string_free(vm->text);
        }, false);

    view_free(instance->view);
    free(instance);
}
//? Basic Functions End

//? Custom Functions
void doc_selection_set_title(DocSelection* instance, const char* title) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        DocSelectionViewModel* vm,
        {
            vm->title = title;
        }, true);
}

FuriString* doc_selection_get_string(DocSelection* instance) {
    furi_assert(instance);

    FuriString* text;
    with_view_model(
        instance->view,
        DocSelectionViewModel* vm,
        {
            text = vm->text;
        }, true);

    return text;
}

void doc_selection_set_footer(DocSelection* instance, const char* footer) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        DocSelectionViewModel* vm,
        {
            vm->footer = footer;
        }, true);
}

void doc_selection_set_index(DocSelection* instance, uint8_t index) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        DocSelectionViewModel* model,
        {
            uint8_t position = index;
            if(position >= model->size) {
                position = 0;
            }

            model->position = position;
            model->window_position = position;

            if(model->window_position > 0) {
                model->window_position -= 1;
            }

            if(model->size <= 3) {
                model->window_position = 0;
            } else {
                if(model->window_position >= model->size - 3) {
                    model->window_position = model->size - 3;
                }
            }
        },
        true);
}

void doc_selection_set_size(DocSelection* instance, uint8_t size) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        DocSelectionViewModel* vm,
        {
            vm->size = size;
        }, true);
}

void doc_selection_request_redraw(DocSelection* instance) {
    furi_assert(instance);
    if(instance->callback != NULL) {
        instance->callback(instance->ctx, (uint8_t) - 1);
    }
}

void doc_selection_force_redraw(DocSelection* instance) {
    furi_assert(instance);
    with_view_model(instance->view, DocSelectionViewModel* vm, { UNUSED(vm); }, true);
}
//? Custom Functions End