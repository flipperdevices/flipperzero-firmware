#pragma once
#include <gui/gui.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>

#ifndef MAX_NUMBER_OF_NOTES
#define MAX_NUMBER_OF_NOTES 10
#define MAX_LENGTH_OF_NOTE 30
#endif

struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    TextInput* text_input;
    char** notes;
    uint8_t number_of_notes;
};

//struct App;

typedef enum {
    Notes_Main_Menu,
    Notes_TextInput,
    Notes_List_of_Notes,
    Notes_Scene_Count
} Scenes; // They are the "pages"

typedef enum {
    Notes_Submenu_View,
    Notes_Widget_View,
    Notes_TextInput_View
} Views; // They are the type of pages

typedef enum { Notes_Main_menu_Add_Item, Notes_Main_menu_List_Item } Submenu_Choices;

typedef enum { Notes_Text_Input_Save_Event } Notes_Input_Event;

void notes_main_menu_callback(void* context, uint32_t index) {
    struct App* app = context;

    scene_manager_handle_custom_event(app->scene_manager, index);
    /*
        switch(index) {
            case BasicScenesMainMenuSceneLottoNumbers:
                scene_manager_handle_custom_event(
                    app->scene_manager,
                    BasicScenesMainMenuSceneLottoNumbersEvent);
                break;
            case BasicScenesMainMenuSceneGreeting:
                scene_manager_handle_custom_event(
                    app->scene_manager,
                    BasicScenesMainMenuSceneGreetingEvent);
                break;
    }
    */
    // it calls the function below
}

void notes_text_input_callback(void* context) {
    struct App* app = context;
    scene_manager_handle_custom_event(app->scene_manager, Notes_Text_Input_Save_Event);
}

void note_main_menu_on_enter(void* context) {
    struct App* app = context;

    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "~~~ Notes ~~~");
    submenu_add_item(
        app->submenu, " Add new note ", Notes_Main_menu_Add_Item, notes_main_menu_callback, app);
    submenu_add_item(
        app->submenu,
        " List your notes ",
        Notes_Main_menu_List_Item,
        notes_main_menu_callback,
        app);
    view_dispatcher_switch_to_view(app->view_dispatcher, Notes_Submenu_View);
}

bool note_main_menu_on_event(void* context, SceneManagerEvent event) {
    struct App* app = context;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case Notes_Main_menu_Add_Item:
            scene_manager_next_scene(app->scene_manager, Notes_TextInput);
            return true;
        case Notes_Main_menu_List_Item:
            scene_manager_next_scene(app->scene_manager, Notes_List_of_Notes);
            return true;
        }
        break;
    default:
        return false;
    }

    return false;
}

void note_main_menu_on_leave(void* context) {
    struct App* app = context;
    submenu_reset(app->submenu);
}

void note_text_input_on_enter(void* context) {
    struct App* app = context;
    bool clear_text = true;
    text_input_reset(app->text_input);
    text_input_set_header_text(app->text_input, "Write down here your note");
    text_input_set_result_callback(
        app->text_input,
        notes_text_input_callback,
        app,
        app->notes[app->number_of_notes],
        MAX_LENGTH_OF_NOTE,
        clear_text);
    app->number_of_notes++;
    view_dispatcher_switch_to_view(app->view_dispatcher, Notes_TextInput_View);
}

bool note_text_input_on_event(void* context, SceneManagerEvent event) {
    struct App* app = context;

    if(event.type == SceneManagerEventTypeCustom && event.event == Notes_Text_Input_Save_Event) {
        scene_manager_next_scene(app->scene_manager, Notes_Main_Menu);
        return true;
    }

    return false;
}

void note_text_input_on_leave(void* context) {
    UNUSED(context);
}

void note_list_of_notes_on_enter(void* context) {
    struct App* app = context;

    widget_reset(app->widget);

    for(int i = 0; i < MAX_NUMBER_OF_NOTES; i++) {
        if(app->notes[i] == NULL || app->notes[i][0] == '\0') {
            continue;
        }

        FuriString* message = furi_string_alloc();
        furi_string_printf(message, "[ #%d ]%s\n", i, app->notes[i]);

        widget_add_string_multiline_element(
            app->widget,
            5,
            5 + (i * 13),
            AlignLeft,
            AlignCenter,
            FontPrimary,
            furi_string_get_cstr(message));

        furi_string_free(message);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, Notes_Widget_View);
}

bool note_list_of_notes_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);

    return 1;
}

void note_list_of_notes_on_leave(void* context) {
    struct App* app = context;
    widget_reset(app->widget);
}

void (*const notes_scene_on_enter_handlers[])(
    void*) = {note_main_menu_on_enter, note_text_input_on_enter, note_list_of_notes_on_enter};

bool (*const notes_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    note_main_menu_on_event,
    note_text_input_on_event,
    note_list_of_notes_on_event};

void (*const notes_scene_on_leave_handlers[])(
    void*) = {note_main_menu_on_leave, note_text_input_on_leave, note_list_of_notes_on_leave};

/*
    custom event => enum
*/
bool notes_custom_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    struct App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

// Handles the back button

bool notes_back_button_handler(void* context) {
    furi_assert(context);
    struct App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static const SceneManagerHandlers notes_scene_manager_handlers = {
    .on_enter_handlers = notes_scene_on_enter_handlers,
    .on_event_handlers = notes_scene_on_event_handlers,
    .on_exit_handlers = notes_scene_on_leave_handlers,
    .scene_num = Notes_Scene_Count};