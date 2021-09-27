#include "loader.h"

#include <furi.h>
#include <furi-hal.h>
#include <cli/cli.h>

#include <gui/view_dispatcher.h>

#include <gui/modules/submenu.h>

#include <menu/menu.h>
#include <menu/menu_item.h>
#include <applications.h>
#include <assets_icons.h>

#define LOADER_LOG_TAG "loader"

struct Loader {
    osThreadId_t loader_thread;
    FuriThread* thread;
    const FlipperApplication* current_app;
    string_t args;
    Cli* cli;
    ValueMutex* menu_vm;
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* primary_submenu;
    Submenu* plugins_menu;
    Submenu* debug_menu;
    Submenu* settings_menu;
    size_t free_heap_size;
    osMutexId_t mutex;
    volatile uint8_t lock_semaphore;
};

typedef enum {
    LoaderMenuViewPrimary,
    LoaderMenuViewPlugins,
    LoaderMenuViewDebug,
    LoaderMenuViewSettings,
} LoaderMenuView;
