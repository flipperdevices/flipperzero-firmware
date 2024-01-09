#include <furi.h>
#include "app/app.h"
#include "scenes/scenes.h"

int32_t crypto_dict_app(void* p);

int main() {
    void* parameter = NULL;
    crypto_dict_app(parameter);
    return 0;
}

int32_t crypto_dict_app(void* p) {
    UNUSED(p);
    App* app = app_alloc();
    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, MainMenuScene);
    view_dispatcher_run(app->view_dispatcher);
    app_free(app);
    return 0;
}