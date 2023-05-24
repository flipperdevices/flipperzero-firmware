#include <furi.h>
#include "app.h"

int note_entry_point(void* p) {
    UNUSED(p);

    struct App* app = initalize_app();

    Gui* gui = furi_record_open(RECORD_GUI);

    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(app->scene_manager, Notes_Main_Menu);
    view_dispatcher_run(app->view_dispatcher);
    /*  It will return when we exit from the app */

    free_app(app);
    return 0;
}