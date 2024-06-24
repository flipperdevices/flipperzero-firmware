#include "example_number_input_show_number.h"

struct ExampleNumberInputShowNumber {
    View* view;
    ExampleNumberInputShowNumberCallback callback;
    void* context;
};

typedef struct {
    FuriString* number;
} ExampleNumberInputShowNumberModel;

void example_number_input_show_number_set_callback(
    ExampleNumberInputShowNumber* instance,
    ExampleNumberInputShowNumberCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void example_number_input_show_number_draw(
    Canvas* canvas,
    ExampleNumberInputShowNumberModel* model) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignTop, "The Number is");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 22, AlignCenter, AlignTop, furi_string_get_cstr(model->number));

    elements_button_left(canvas, "Min");
    elements_button_right(canvas, "Max");
    elements_button_center(canvas, "Change");
}

static void
    example_number_input_show_number_model_init(ExampleNumberInputShowNumberModel* const model) {
    model->number = furi_string_alloc();
}

void example_number_input_show_number_model_set_number(
    ExampleNumberInputShowNumber* instance,
    int32_t number) {
    furi_assert(instance);
    ExampleNumberInputShowNumberModel* model = view_get_model(instance->view);
    furi_string_printf(model->number, "%ld", number);
    view_commit_model(instance->view, false);
}

bool example_number_input_show_number_input(InputEvent* event, void* context) {
    furi_assert(context);
    ExampleNumberInputShowNumber* instance = context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                ExampleNumberInputShowNumberModel * model,
                {
                    UNUSED(model);
                    instance->callback(
                        ExampleNumberInputCustomEventShowNumberBack, instance->context);
                },
                true);
            break;
        case InputKeyOk:
            with_view_model(
                instance->view,
                ExampleNumberInputShowNumberModel * model,
                {
                    UNUSED(model);
                    instance->callback(
                        ExampleNumberInputCustomEventShowNumberOk, instance->context);
                },
                true);
            break;
        case InputKeyLeft:
            with_view_model(
                instance->view,
                ExampleNumberInputShowNumberModel * model,
                {
                    UNUSED(model);
                    instance->callback(
                        ExampleNumberInputCustomEventShowNumberLeft, instance->context);
                },
                true);
            break;
        case InputKeyRight:
            with_view_model(
                instance->view,
                ExampleNumberInputShowNumberModel * model,
                {
                    UNUSED(model);
                    instance->callback(
                        ExampleNumberInputCustomEventShowNumberRight, instance->context);
                },
                true);
            break;
        default:
            break;
        }
    }
    return true;
}

void example_number_input_show_number_exit(void* context) {
    furi_assert(context);
}

void example_number_input_show_number_enter(void* context) {
    furi_assert(context);
}

ExampleNumberInputShowNumber* example_number_input_show_number_alloc() {
    ExampleNumberInputShowNumber* instance = malloc(sizeof(ExampleNumberInputShowNumber));
    instance->view = view_alloc();
    view_allocate_model(
        instance->view, ViewModelTypeLocking, sizeof(ExampleNumberInputShowNumberModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(
        instance->view, (ViewDrawCallback)example_number_input_show_number_draw);
    view_set_input_callback(instance->view, example_number_input_show_number_input);

    with_view_model(
        instance->view,
        ExampleNumberInputShowNumberModel * model,
        { example_number_input_show_number_model_init(model); },
        true);

    return instance;
}

void example_number_input_show_number_free(ExampleNumberInputShowNumber* instance) {
    furi_assert(instance);
    with_view_model(
        instance->view,
        ExampleNumberInputShowNumberModel * model,
        { furi_string_free(model->number); },
        true);
    view_free(instance->view);
    free(instance);
}

View* example_number_input_show_number_get_view(ExampleNumberInputShowNumber* instance) {
    furi_assert(instance);
    return instance->view;
}
