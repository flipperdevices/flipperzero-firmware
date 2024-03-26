#include "doc_i.h"
#include "helpers/doc_storage_helper.h"

#include <dialogs/dialogs.h>
#include <scenes/doc_scene.h>

bool doc_custom_event_callback(void* ctx, uint32_t event) {
    furi_assert(ctx);
    Doc* app = ctx;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

bool doc_back_event_callback(void* ctx) {
    furi_assert(ctx);
    Doc* app = ctx;
    return scene_manager_handle_back_event(app->scene_manager);
}

/*
 * The Dolphin service only saves 30s after the latest change,
 * it can take up to that amount to get the real current values.
 * The timer gets called after 10s, for 3 times and then stops.
 */
void doc_dolphin_timer_callback(void* ctx) {
    furi_assert(ctx);
    Doc* app = ctx;

    app->dolphin_timer_counter++;

    if(app->dolphin_timer_counter <= 3) {
        FURI_LOG_D(TAG, "Loading new state after %hhus", app->dolphin_timer_counter * 10);
        doc_dolphin_state_load(app->dolphin);
        furi_delay_ms(20);

        if(app->in_selection) {
            doc_selection_request_redraw(app->selection);
        }
        if(app->in_description) {
            doc_description_request_redraw(app->description);
        }
    }

    if(app->dolphin_timer_counter == 3) {
        FURI_LOG_D(TAG, "30s reached, stopping timer.");
        furi_timer_stop(app->dolphin_timer);
    }
}

Doc* doc_alloc() {
    Doc* app = malloc(sizeof(Doc));

    app->dolphin = malloc(sizeof(DolphinState));
    app->dolphin_timer = furi_timer_alloc(doc_dolphin_timer_callback, FuriTimerTypePeriodic, app);
    app->file_path = furi_string_alloc();

    //? ------------- Records -------------
    app->gui = furi_record_open(RECORD_GUI);
    app->dialogs = furi_record_open(RECORD_DIALOGS);
    //? ----------- Records End -----------

    // ViewDispatcher & SceneManager
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&doc_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, doc_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, doc_back_event_callback);

    //! -------------- DEBUG --------------
    app->notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(app->notification, &sequence_display_backlight_on);
    //! ------------ DEBUG END ------------
    
    //? -------------- Views --------------
    app->selection = doc_selection_alloc();
    view_dispatcher_add_view(app->view_dispatcher, DocSelectionView, doc_selection_get_view(app->selection));
    app->description = doc_description_alloc();
    view_dispatcher_add_view(app->view_dispatcher, DocDescriptionView, doc_description_get_view(app->description));
    app->text_input = text_input_alloc();
    view_dispatcher_add_view(app->view_dispatcher, DocTextInputView, text_input_get_view(app->text_input));
    //? ------------ Views End ------------
    
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    return app;
}

void doc_free(Doc* app) {

    //! -------------- DEBUG --------------
    furi_record_close(RECORD_NOTIFICATION);
    app->notification = NULL;
    //! ------------ DEBUG END ------------
    
    //? -------------- Views --------------
    view_dispatcher_remove_view(app->view_dispatcher, DocSelectionView);
    doc_selection_free(app->selection);
    view_dispatcher_remove_view(app->view_dispatcher, DocDescriptionView);
    doc_description_free(app->description);
    view_dispatcher_remove_view(app->view_dispatcher, DocTextInputView);
    text_input_free(app->text_input);
    //? ------------ Views End ------------

    // ViewDispatcher & SceneManager
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    //? ------------- Records -------------
    furi_record_close(RECORD_GUI);
    app->gui = NULL;
    furi_record_close(RECORD_DIALOGS);
    app->dialogs = NULL;
    //? ----------- Records End -----------

    furi_string_free(app->file_path);
    furi_timer_free(app->dolphin_timer);
    free(app->dolphin);
    
    free(app);
}

uint32_t doc_app(void* p) {
    UNUSED(p);
    Doc* app = doc_alloc();

    doc_dolphin_state_load(app->dolphin);
    furi_timer_start(app->dolphin_timer, 10 * 1000);
    scene_manager_next_scene(app->scene_manager, DocSceneMenu);
    view_dispatcher_run(app->view_dispatcher);

    doc_free(app);
    return 0;
}