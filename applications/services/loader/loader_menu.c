#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/menu.h>
#include <gui/modules/submenu.h>
#include <assets_icons.h>
#include <applications.h>
#include <cfw.h>

#include "loader.h"
#include "loader_menu.h"

#define TAG "LoaderMenu"

struct LoaderMenu {
    FuriThread* thread;
    void (*closed_cb)(void*);
    void* context;
};

static int32_t loader_menu_thread(void* p);
static int32_t loader_gamesmenu_thread(void* p);

LoaderMenu* loader_menu_alloc(void (*closed_cb)(void*), void* context) {
    LoaderMenu* loader_menu = malloc(sizeof(LoaderMenu));
    loader_menu->closed_cb = closed_cb;
    loader_menu->context = context;
    loader_menu->thread = furi_thread_alloc_ex(TAG, 1024, loader_menu_thread, loader_menu);
    furi_thread_start(loader_menu->thread);
    return loader_menu;
}

LoaderMenu* loader_gamesmenu_alloc(void (*closed_cb)(void*), void* context) {
    LoaderMenu* loader_menu = malloc(sizeof(LoaderMenu));
    loader_menu->closed_cb = closed_cb;
    loader_menu->context = context;
    loader_menu->thread = furi_thread_alloc_ex(TAG, 1024, loader_gamesmenu_thread, loader_menu);
    furi_thread_start(loader_menu->thread);
    return loader_menu;
}

void loader_menu_free(LoaderMenu* loader_menu) {
    furi_assert(loader_menu);
    furi_thread_join(loader_menu->thread);
    furi_thread_free(loader_menu->thread);
    free(loader_menu);
}

typedef enum {
    LoaderMenuViewPrimary,
    LoaderMenuViewSettings,
} LoaderMenuView;

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Menu* primary_menu;
    Submenu* settings_menu;
} LoaderMenuApp;

static void loader_menu_start(const char* name) {
    Loader* loader = furi_record_open(RECORD_LOADER);
    loader_start_with_gui_error(loader, name, NULL);
    furi_record_close(RECORD_LOADER);
}

static void loader_menu_callback(void* context, uint32_t index) {
    UNUSED(context);
    loader_menu_start((const char*)index);
}

static void loader_menu_switch_to_settings(void* context, uint32_t index) {
    UNUSED(index);
    LoaderMenuApp* app = context;
    view_dispatcher_switch_to_view(app->view_dispatcher, LoaderMenuViewSettings);
}

static uint32_t loader_menu_switch_to_primary(void* context) {
    UNUSED(context);
    return LoaderMenuViewPrimary;
}

static uint32_t loader_menu_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static void loader_menu_build_menu(LoaderMenuApp* app, LoaderMenu* menu) {
    Loader* loader = furi_record_open(RECORD_LOADER);
    MainMenuList_t* mainmenu_apps = loader_get_mainmenu_apps(loader);
    furi_record_close(RECORD_LOADER);

    // Apps from CFW Settings
    for(size_t i = 0; i < MainMenuList_size(*mainmenu_apps); i++) {
        const MainMenuApp* mainmenu_app = MainMenuList_get(*mainmenu_apps, i);

        if(strcmp(mainmenu_app->name, LOADER_APPLICATIONS_NAME) == 0) {
            menu_add_item(
                app->primary_menu,
                LOADER_APPLICATIONS_NAME,
                &A_Plugins_14,
                (uint32_t)LOADER_APPLICATIONS_NAME,
                loader_menu_callback,
                (void*)menu);
        } else if(strcmp(mainmenu_app->name, "Settings") == 0) {
            menu_add_item(
                app->primary_menu,
                "Settings",
                &A_Settings_14,
                (uint32_t) "Settings",
                loader_menu_switch_to_settings,
                app);
        } else {
            menu_add_item(
                app->primary_menu,
                mainmenu_app->name,
                mainmenu_app->icon,
                (uint32_t)mainmenu_app->path,
                loader_menu_callback,
                (void*)menu);
        }
    }
}

static void loader_menu_build_gamesmenu(LoaderMenuApp* app, LoaderMenu* menu) {
    Loader* loader = furi_record_open(RECORD_LOADER);
    GamesMenuList_t* gamesmenu_apps = loader_get_gamesmenu_apps(loader);
    furi_record_close(RECORD_LOADER);

    // Game Apps from CFW Settings
    for(size_t i = 0; i < GamesMenuList_size(*gamesmenu_apps); i++) {
        const GamesMenuApp* gamesmenu_app = GamesMenuList_get(*gamesmenu_apps, i);

        menu_add_item(
            app->primary_menu,
            gamesmenu_app->name,
            gamesmenu_app->icon,
            (uint32_t)gamesmenu_app->path,
            loader_menu_callback,
            (void*)menu);
    }
}

static void loader_menu_build_submenu(LoaderMenuApp* app, LoaderMenu* loader_menu) {
    for(size_t i = 0; i < FLIPPER_SETTINGS_APPS_COUNT; i++) {
        submenu_add_item(
            app->settings_menu,
            FLIPPER_SETTINGS_APPS[i].name,
            (uint32_t)FLIPPER_SETTINGS_APPS[i].name,
            loader_menu_callback,
            loader_menu);
    }
}

static LoaderMenuApp* loader_menu_app_alloc(LoaderMenu* loader_menu) {
    LoaderMenuApp* app = malloc(sizeof(LoaderMenuApp));
    app->gui = furi_record_open(RECORD_GUI);
    app->view_dispatcher = view_dispatcher_alloc();

    Loader* loader = furi_record_open(RECORD_LOADER);
    MainMenuList_t* mainmenu_apps = loader_get_mainmenu_apps(loader);
    size_t APP_COUNT = MainMenuList_size(*mainmenu_apps);
    furi_record_close(RECORD_LOADER);

    uint32_t my_start_point = CLAMP(CFW_SETTINGS()->start_point, APP_COUNT - 1, 0U);
    app->primary_menu = menu_pos_alloc((size_t)my_start_point);
    app->settings_menu = submenu_alloc();

    loader_menu_build_menu(app, loader_menu);
    loader_menu_build_submenu(app, loader_menu);

    // Primary menu
    View* primary_view = menu_get_view(app->primary_menu);
    view_set_context(primary_view, app->primary_menu);
    view_set_previous_callback(primary_view, loader_menu_exit);
    view_dispatcher_add_view(app->view_dispatcher, LoaderMenuViewPrimary, primary_view);

    // Settings menu
    View* settings_view = submenu_get_view(app->settings_menu);
    view_set_context(settings_view, app->settings_menu);
    view_set_previous_callback(settings_view, loader_menu_switch_to_primary);
    view_dispatcher_add_view(app->view_dispatcher, LoaderMenuViewSettings, settings_view);

    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_switch_to_view(app->view_dispatcher, LoaderMenuViewPrimary);

    return app;
}

static LoaderMenuApp* loader_gamesmenu_app_alloc(LoaderMenu* loader_menu) {
    LoaderMenuApp* app = malloc(sizeof(LoaderMenuApp));
    app->gui = furi_record_open(RECORD_GUI);
    app->view_dispatcher = view_dispatcher_alloc();

    app->primary_menu = menu_alloc();
    loader_menu_build_gamesmenu(app, loader_menu);

    // Primary menu
    View* primary_view = menu_get_view(app->primary_menu);
    view_set_context(primary_view, app->primary_menu);
    view_set_previous_callback(primary_view, loader_menu_exit);
    view_dispatcher_add_view(app->view_dispatcher, LoaderMenuViewPrimary, primary_view);

    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_switch_to_view(app->view_dispatcher, LoaderMenuViewPrimary);

    return app;
}

static void loader_menu_app_free(LoaderMenuApp* app) {
    view_dispatcher_remove_view(app->view_dispatcher, LoaderMenuViewPrimary);
    view_dispatcher_remove_view(app->view_dispatcher, LoaderMenuViewSettings);
    view_dispatcher_free(app->view_dispatcher);

    menu_free(app->primary_menu);
    submenu_free(app->settings_menu);
    furi_record_close(RECORD_GUI);
    free(app);
}

static void loader_gamesmenu_app_free(LoaderMenuApp* app) {
    view_dispatcher_remove_view(app->view_dispatcher, LoaderMenuViewPrimary);
    view_dispatcher_free(app->view_dispatcher);

    menu_free(app->primary_menu);
    furi_record_close(RECORD_GUI);
    free(app);
}

static int32_t loader_menu_thread(void* p) {
    LoaderMenu* loader_menu = p;
    furi_assert(loader_menu);

    LoaderMenuApp* app = loader_menu_app_alloc(loader_menu);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_run(app->view_dispatcher);

    if(loader_menu->closed_cb) {
        loader_menu->closed_cb(loader_menu->context);
    }

    loader_menu_app_free(app);

    return 0;
}

static int32_t loader_gamesmenu_thread(void* p) {
    LoaderMenu* loader_menu = p;
    furi_assert(loader_menu);

    LoaderMenuApp* app = loader_gamesmenu_app_alloc(loader_menu);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_run(app->view_dispatcher);

    if(loader_menu->closed_cb) {
        loader_menu->closed_cb(loader_menu->context);
    }

    loader_gamesmenu_app_free(app);

    return 0;
}