#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_usb_cdc.h>
#include <furi_hal_usb.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/text_box.h>
#include <gui/modules/text_input.h>
#include <m-dict.h>
#include <loader/loader.h>

DICT_DEF2(ViewDict, uint32_t, M_DEFAULT_OPLIST, View*, M_PTR_OPLIST)

#define TEXT_BOX_STORE_SIZE (4096)
#define TEXT_INPUT_STORE_SIZE (512)

typedef struct {
    FuriMessageQueue* queue;
    Gui* gui;
    ViewPort* view_port;
    ViewDict_t views;

    View* current_view;

    View* ongoing_input_view;
    uint8_t ongoing_input;

    ViewDispatcherCustomEventCallback custom_event_callback;
    ViewDispatcherNavigationEventCallback navigation_event_callback;
    ViewDispatcherTickEventCallback tick_event_callback;
    uint32_t tick_period;
    void* event_context;
} ViewDispatcher_internal;
typedef struct {
    Gui* gui;
    bool is_enabled;
    ViewPortOrientation orientation;

    uint8_t width;
    uint8_t height;

    ViewPortDrawCallback draw_callback;
    void* draw_callback_context;

    ViewPortInputCallback input_callback;
    void* input_callback_context;
} ViewPort_internal;
typedef struct {
    FuriThreadId loader_thread;

    const void* application;
    FuriThread* application_thread;
    char* application_arguments;

    void* cli;
    void* gui;

    void* view_dispatcher;
    void* primary_menu;
    void* plugins_menu;
    void* debug_menu;
    void* settings_menu;

    volatile uint8_t lock_count;

    void* pubsub;
} Loader_internal;

typedef enum {
    ViewTextInput,
    ViewConsoleOutput,
} CliguiState;

typedef struct {
    CliguiState state;
    struct {
        FuriStreamBuffer* app_tx;
        FuriStreamBuffer* app_rx;
    } streams;
} CliguiData;

typedef struct {
    CliguiData* data;
    Gui* gui;
    TextBox* text_box;
    FuriString* text_box_store;
    char text_input_store[TEXT_INPUT_STORE_SIZE + 1];
    TextInput* text_input;
    ViewDispatcher* view_dispatcher;
    ViewDispatcher_internal* view_dispatcher_i;
} CliguiApp;