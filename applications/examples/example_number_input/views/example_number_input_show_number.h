#pragma once

#include <gui/view.h>
#include <gui/elements.h>
#include "../example_number_input_custom_event.h"

typedef struct ExampleNumberInputShowNumber ExampleNumberInputShowNumber;

typedef void (
    *ExampleNumberInputShowNumberCallback)(ExampleNumberInputCustomEvent event, void* context);

void example_number_input_show_number_model_set_number(
    ExampleNumberInputShowNumber* instance,
    int32_t number);
void example_number_input_show_number_set_callback(
    ExampleNumberInputShowNumber* example_number_input_show_number,
    ExampleNumberInputShowNumberCallback callback,
    void* context);

View* example_number_input_show_number_get_view(ExampleNumberInputShowNumber* instance);

ExampleNumberInputShowNumber* example_number_input_show_number_alloc();

void example_number_input_show_number_free(ExampleNumberInputShowNumber* instance);