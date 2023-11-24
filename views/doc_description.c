#include "doc_description.h"
#include "doc_view_common.h"

#include <doc_icons.h>

struct DocDescription {
    View* view;
    DocDescriptionCallback callback;
    void* ctx;
};

typedef struct {
    FuriString* text;

    uint8_t category;

    //Internal
    uint8_t size;
    uint8_t index;
} DocDescriptionViewModel;

static void doc_description_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    DocDescriptionViewModel* vm = ctx;

    canvas_draw_icon(canvas, 0, 0, &I_doc_bigscreen_light);

    // Scrolling Arrow
    if(vm->index > 0) {
        canvas_draw_icon(canvas, 113, 13, &I_doc_button_up);
    }
    if(vm->size > 4 &&
        vm->index < vm->size - 4) {
        canvas_draw_icon(canvas, 113, 39, &I_doc_button_down);
    }

    // Text
    doc_draw_text(canvas, vm->text, 18, 110, 16, 10, vm->index, 4);
}

static bool doc_description_input_callback(InputEvent* event, void* ctx) {
    furi_assert(ctx);
    DocDescription* instance = ctx;
    bool consumed = false;

    if(event->type == InputTypeShort || event->type == InputTypeRepeat) {
        switch(event->key) {
        case InputKeyUp:
            with_view_model(
                instance->view,
                DocDescriptionViewModel* model,
                {
                    if(model->index > 0) {
                        model->index--;
                    }
                },
                true);
            consumed = true;
            break;
        case InputKeyDown:
            with_view_model(
                instance->view,
                DocDescriptionViewModel* model,
                {
                    if(model->index < model->size - 4) {
                        model->index++;
                    }
                },
                true);
            consumed = true;
            break;
        case InputKeyBack:
            with_view_model(
                instance->view,
                DocDescriptionViewModel* model,
                {
                    model->index = 0;
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
View* doc_description_get_view(DocDescription* instance) {
    furi_assert(instance);
    return instance->view;
}

void doc_description_set_callback(DocDescription* instance, DocDescriptionCallback callback, void* ctx) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->ctx = ctx;
}

DocDescription* doc_description_alloc() {
    DocDescription* instance = malloc(sizeof(DocDescription));

    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(DocDescriptionViewModel));
    view_set_draw_callback(instance->view, doc_description_draw_callback);
    view_set_input_callback(instance->view, doc_description_input_callback);
    view_set_context(instance->view, instance);

    with_view_model(
        instance->view,
        DocDescriptionViewModel* vm,
        {
            vm->text = furi_string_alloc();
        }, false);

    return instance;
}

void doc_description_free(DocDescription* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        DocDescriptionViewModel* vm,
        {
            furi_string_free(vm->text);
        }, false);

    view_free(instance->view);
    free(instance);
}
//? Basic Functions End

//? Custom Functions
FuriString* doc_description_get_string(DocDescription* instance) {
    furi_assert(instance);

    FuriString* text;
    with_view_model(
        instance->view,
        DocDescriptionViewModel* vm,
        {
            text = vm->text;
        }, true);

    return text;
}

uint8_t doc_description_get_category(DocDescription* instance) {
    furi_assert(instance);

    uint8_t category;
    with_view_model(
        instance->view,
        DocDescriptionViewModel* vm,
        {
            category = vm->category;
        }, false);

    return category;
}

void doc_description_set_category(DocDescription* instance, uint8_t category) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        DocDescriptionViewModel* vm,
        {
            vm->category = category;
        }, true);
}

void doc_description_set_size(DocDescription* instance, uint8_t size) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        DocDescriptionViewModel* vm,
        {
            vm->size = size;
        }, true);
}

void doc_description_request_redraw(DocDescription* instance) {
    furi_assert(instance);
    if(instance->callback != NULL) {
        instance->callback(instance->ctx, (uint8_t) - 1);
    }
}

void doc_description_force_redraw(DocDescription* instance) {
    furi_assert(instance);
    with_view_model(instance->view, DocDescriptionViewModel* vm, { UNUSED(vm); }, true);
}
//? Custom Functions End