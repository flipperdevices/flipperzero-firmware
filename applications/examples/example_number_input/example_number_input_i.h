#pragma once

#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/elements.h>
#include <gui/scene_manager.h>
//#include <gui/modules/number_input.h>
#include "temp/number_input.h"
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include "example_number_input_custom_event.h"
#include "scenes/example_number_input_scene.h"
#include "views/example_number_input_show_number.h"

#define EXAMPLE_TEXT_STORE_NUM 3
#define EXAMPLE_TEXT_STORE_SIZE 128

typedef struct ExampleNumberInputShowNumber ExampleNumberInputShowNumber;

typedef enum {
    ExampleNumberInputViewIdShowNumber,
    ExampleNumberInputViewIdNumberInput,
    //ExampleNumberInputViewIdStack,
} ExampleNumberInputViewId;

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    FuriString* text_buffer;
    NumberInput* number_input;
    ExampleNumberInputShowNumber* show_number;
} ExampleNumberInput;