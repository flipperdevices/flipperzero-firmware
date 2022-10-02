#ifndef FAP_LOADER_APP_H
#define FAP_LPADER_APP_H

#include <gui/modules/loading.h>
#include <dialogs/dialogs.h>
#include <flipper_application/flipper_application.h>

#define TAG "fap_loader_app"

typedef struct {
    FlipperApplication* app;
    Storage* storage;
    DialogsApp* dialogs;
    Gui* gui;
    string_t fap_path;

    ViewDispatcher* view_dispatcher;
    Loading* loading;
} FapLoader;

bool fap_loader_item_callback(string_t path, void* context, uint8_t** icon_ptr, string_t item_name);

#endif  // FAP_LPADER_APP_H
