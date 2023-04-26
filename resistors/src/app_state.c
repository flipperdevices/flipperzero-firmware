#include "flipper.h"
#include "resistors_app.h"
#include "app_state.h"
#include "scenes.h"

App* app_alloc() {
    App* app = malloc(sizeof(App));
    app->scene_manager = scene_manager_alloc(&resistors_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, resistors_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, resistors_back_event_callback);

    app->submenu = submenu_alloc();
    app->widget = widget_alloc();

    view_dispatcher_add_view(
        app->view_dispatcher, ResistorsSubmenuView, submenu_get_view(app->submenu));
    view_dispatcher_add_view(
        app->view_dispatcher, ResistorsEditView, widget_get_view(app->widget));

    return app;
}

AppState* app_state_alloc() {
    AppState* state = malloc(sizeof(AppState));
    return state;
}

void app_init_resistor(App* app, ResistorType rtype) {
    app->state->resistor_type = rtype;
    app->state->edit_selection = 0;
    switch(rtype) {
    case Resistor4Band:
        app->state->resistor_bands[0] = BandRed;
        app->state->resistor_bands[1] = BandOrange;
        app->state->resistor_bands[2] = BandYellow;
        app->state->resistor_bands[3] = BandGreen;
        break;
    case Resistor5Band:
        app->state->resistor_bands[0] = BandRed;
        app->state->resistor_bands[1] = BandOrange;
        app->state->resistor_bands[2] = BandYellow;
        app->state->resistor_bands[3] = BandGreen;
        app->state->resistor_bands[4] = BandBlue;
        break;
    default:
        FURI_LOG_E(TAG, "Unknown resistor type in app_init_resistor");
        app_quit(app);
        break;
    }
}

void app_quit(App* app) {
    scene_manager_stop(app->scene_manager);
}

void app_free(App* app) {
    furi_assert(app);

    free(app->state);

    view_dispatcher_remove_view(app->view_dispatcher, ResistorsSubmenuView);
    view_dispatcher_remove_view(app->view_dispatcher, ResistorsEditView);

    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    submenu_free(app->submenu);
    widget_free(app->widget);

    free(app);
}
