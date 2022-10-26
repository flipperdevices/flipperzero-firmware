#pragma once

#include <furi.h>
#include <furi_hal_spi.h>
#include <furi_hal_spi_config.h>
#include "spi_mem_app.h"
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include <notification/notification_messages.h>
#include <flipper_format/flipper_format.h>
#include <dialogs/dialogs.h>
#include <gui/modules/widget.h>
#include <gui/modules/text_input.h>
#include <storage/storage.h>
#include <toolbox/path.h>
#include <toolbox/random_name.h>
#include "scenes/spi_mem_scene.h"
#include "lib/spi/spi_mem_worker.h"
#include "spi_mem_manager_icons.h"
#include "views/spi_mem_view_read.h"

#define TAG "SPIMem"
#define SPI_MEM_FILE_TYPE "Flipper SPI memmory dump"
#define SPI_MEM_FILE_EXTENSION ".spimem"
#define SPI_MEM_FILE_FOLDER EXT_PATH("spimem")
#define SPI_MEM_FILE_NAME_SIZE 100
#define SPI_MEM_TEXT_BUFFER_SIZE 128

struct SPIMemApp {
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
    TextInput* text_input;
    char text_buffer[SPI_MEM_TEXT_BUFFER_SIZE + 1];
};

typedef enum {
    SPIMemViewSubmenu,
    SPIMemViewDialogEx,
    SPIMemViewPopup,
    SPIMemViewWidget,
    SPIMemViewTextInput,
    SPIMemViewRead
} SPIMemView;

typedef enum {
    SPIMemCustomEventViewReadCancel,
    SPIMemCustomEventTextEditResult,
    SPIMemCustomEventPopupBack
} SPIMemCustomEvent;
