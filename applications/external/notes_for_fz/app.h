#pragma once
#include "view.h"

#ifndef MAX_NUMBER_OF_NOTES
#define MAX_NUMBER_OF_NOTES 10
#define MAX_LENGTH_OF_NOTE 30
#endif

static struct App* initalize_app(void) {
    struct App* app = malloc(sizeof(struct App));
    app->number_of_notes = 0;

    /* Allocate memory for all the notes */

    app->notes = (char**)malloc(MAX_LENGTH_OF_NOTE * sizeof(char*) * MAX_NUMBER_OF_NOTES);
    for(int i = 0; i < MAX_NUMBER_OF_NOTES; i++) {
        app->notes[i] = (char*)malloc(MAX_LENGTH_OF_NOTE * sizeof(char));
    }

    app->scene_manager = scene_manager_alloc(&notes_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, notes_custom_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, notes_back_button_handler);
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, Notes_Submenu_View, submenu_get_view(app->submenu));
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, Notes_Widget_View, widget_get_view(app->widget));
    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, Notes_TextInput_View, text_input_get_view(app->text_input));
    return app;
}

void free_app(struct App* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->view_dispatcher, Notes_Submenu_View);
    view_dispatcher_remove_view(app->view_dispatcher, Notes_Widget_View);
    view_dispatcher_remove_view(app->view_dispatcher, Notes_TextInput_View);
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    widget_free(app->widget);
    text_input_free(app->text_input);
    free(app);
}