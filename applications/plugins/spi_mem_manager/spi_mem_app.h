#pragma once

#include <furi.h>
#include <furi_hal_spi.h>
#include <furi_hal_spi_config.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include <notification/notification_messages.h>
#include <dialogs/dialogs.h>
#include <gui/modules/widget.h>
#include <storage/storage.h>
#include "scenes/spi_mem_scene.h"
#include "lib/spi/spi_mem_worker.h"
#include "spi_mem_manager_icons.h"
#include "views/spi_mem_view_read.h"

#define SPI_MEM_FILE_EXTENSION ".spimem"
#define SPI_MEM_FILE_FOLDER ANY_PATH("spimem")

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    Submenu* submenu;
    DialogEx* dialog_ex;
    Popup* popup;
    NotificationApp* notifications;
    FuriString* file_path;
    DialogsApp* dialogs;
    Storage* storage;
    Widget* widget;
    SPIMemWorker* worker;
    SPIMemChip* chip_info;
    SPIMemReadView* view_read;
} SPIMemApp;

typedef enum {
    SPIMemViewSubmenu,
    SPIMemViewDialogEx,
    SPIMemViewPopup,
    SPIMemViewWidget,
    SPIMemViewRead
} SPIMemView;

typedef enum { SPIMemCustomEventViewReadCancel } SPIMemCustomEvent;

void spi_mem_file_create_folder(SPIMemApp* app);
bool spi_mem_file_select(SPIMemApp* app);
