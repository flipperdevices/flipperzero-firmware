#include "flipper.h"
#include "resistors_app.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_main_menu.h"
#include "scene_edit.h"

/* entrypoint */

int32_t resistors_app(void* p) {
    UNUSED(p);
    FURI_LOG_I("TEST", "resistors app launched");
    App* app = app_alloc();
    app->state = app_state_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, ResistorsMainMenuScene);
    view_dispatcher_run(app->view_dispatcher);

    app_free(app);
    return 0;
}
