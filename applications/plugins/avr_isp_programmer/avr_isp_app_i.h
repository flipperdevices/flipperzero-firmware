#pragma once

#include "helpers/avr_isp_types.h"
#include <avr_isp_icons.h>

#include "scenes/avr_isp_scene.h"
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>
#include <gui/modules/text_input.h>
#include <dialogs/dialogs.h>
#include <storage/storage.h>
//#include <gui/modules/variable_item_list.h>

#include "views/avr_isp_view_programmer.h"
#include "views/avr_isp_view_reader.h"
#include "views/avr_isp_view_writer.h"

#define AVR_ISP_MAX_LEN_NAME 64

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    DialogsApp* dialogs;
    //VariableItemList* variable_item_list;
    Submenu* submenu;
    Widget* widget;
    TextInput* text_input;
    FuriString* file_path;
    //FuriString* file_name;
    char file_name_tmp[AVR_ISP_MAX_LEN_NAME];
    AvrIspProgrammerView* avr_isp_programmer_view;
    AvrIspReaderView* avr_isp_reader_view;
    AvrIspWriterView* avr_isp_writer_view;
} AvrIspApp;

bool avr_isp_load_from_file(AvrIspApp* app);