#include "questions.h"

#define TAG "QuestionsApp"

void q_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    QuestionApp* app = context;
    q_reset_view(app->q_main_view, index);
    view_dispatcher_switch_to_view(app->view_dispatcher, QuestionViewMain);
}

uint32_t q_go_to_menu(void* context) {
    UNUSED(context);
    return QuestionViewSubmenu;
}

uint32_t q_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

QuestionApp* q_app_alloc() {
    QuestionApp* app = malloc(sizeof(QuestionApp));

    // Gui
    app->gui = furi_record_open(RECORD_GUI);

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Submenu view
    app->submenu = submenu_alloc();
    for(uint8_t i = 0; i < NUMBER_OF_GROUPS; i++) {
        submenu_add_item(app->submenu, Q_GROUPS[i].name, i, q_submenu_callback, app);
    }
    view_set_previous_callback(submenu_get_view(app->submenu), q_exit);
    view_dispatcher_add_view(
        app->view_dispatcher, QuestionViewSubmenu, submenu_get_view(app->submenu));

    app->q_main_view = q_main_view_alloc();
    view_set_previous_callback(app->q_main_view->view, q_go_to_menu);
    view_dispatcher_add_view(app->view_dispatcher, QuestionViewMain, app->q_main_view->view);

    view_dispatcher_switch_to_view(app->view_dispatcher, QuestionViewSubmenu);

    return app;
}

void q_app_free(QuestionApp* app) {
    furi_assert(app);

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, QuestionViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_remove_view(app->view_dispatcher, QuestionViewMain);
    q_main_view_free(app->q_main_view);
    view_dispatcher_free(app->view_dispatcher);

    // Close records
    furi_record_close(RECORD_GUI);
    app->gui = NULL;

    // Free rest
    free(app);
}

int32_t questions_app(void* p) {
    UNUSED(p);
    QuestionApp* app = q_app_alloc();

    view_dispatcher_run(app->view_dispatcher);

    q_app_free(app);
    return 0;
}