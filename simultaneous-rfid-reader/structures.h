#pragma once
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <gui/elements.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>
#include <toolbox/path.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helpers/uart_helper.h"


//Submenu enums for different screens
typedef enum {
    UHFReaderSubmenuIndexRead,
    UHFReaderSubmenuIndexDelete,
    UHFReaderSubmenuIndexTagInfo,
    UHFReaderSubmenuIndexTagRename,
    UHFReaderSubmenuIndexTagWrite,
    UHFReaderSubmenuIndexTagDelete,
    UHFReaderSubmenuIndexSaved,
    UHFReaderSubmenuIndexConfig,
    UHFReaderSubmenuIndexAbout,
    UHFReaderSubmenuIndexStartReading,
    UHFReaderSubmenuIndexStartWriting,
    UHFReaderSubmenuIndexSelectTag,
    UHFReaderSubmenuIndexSetPower,
    UHFReaderSubmenuIndexTagAction,
} UHFReaderSubmenuIndex;

//Defining views for the application
typedef enum {
    UHFReaderViewSubmenu, 
    UHFReaderViewDeleteSuccess,
    UHFReaderViewConfigure, 
    UHFReaderViewEPCDump, 
    UHFReaderViewEPCInfo,
    UHFReaderViewDelete,
    UHFReaderViewAbout, 
    UHFReaderViewRead, 
    UHFReaderViewSaveInput, 
    UHFReaderViewRenameInput,
    UHFReaderViewEPCWriteInput,
    UHFReaderViewWrite, 
    UHFReaderViewSaved,
    UHFReaderViewTagAction,
    UHFReaderViewSelectSavedTag, 
    UHFReaderViewSetPower, 
} UHFReaderView;

//Event IDs for the app
typedef enum {
    UHFReaderEventIdRedrawScreen = 0, 
    UHFReaderEventIdOkPressed = 42, 
} UHFReaderEventId;

//State of the reader app when communicating with raspberry pi zero over uart
typedef enum {
    UHFReaderState_Idle,
    UHFReaderState_WaitForNumber,
    UHFReaderState_CollectEPCs,
    UHFReaderState_DoneCollecting,
    UHFReaderState_WaitForTID,
    UHFReaderState_CollectTIDs,
    UHFReaderState_DoneCollectingTIDs,
    UHFReaderState_WaitForRES,
    UHFReaderState_CollectRESs,
    UHFReaderState_DoneCollectingRESs,
    UHFReaderState_WaitForMEM,
    UHFReaderState_CollectMEMs,
    UHFReaderState_DoneCollectingMEMs
} UHFReaderState;


//The main UHFReaderApp Struct
typedef struct {
    ViewDispatcher* view_dispatcher; 
    NotificationApp* notifications; 
    
    Submenu* submenu; 
    Submenu* submenu_saved; 
    Submenu* submenu_tag_actions; 

    TextInput* text_input; 
    TextInput* save_input; 
    TextInput* rename_input; 
    TextInput* epc_write; 

    VariableItemList* variable_item_list_config; 
    VariableItem* setting_2_item;
    Widget* widget_about;
    
    View* view_read;
    View* view_write; 
    View* view_delete; 
    View* view_delete_success;
    View* view_epc;
    View* view_epc_info; 
    
    char* temp_buffer; 
    char* temp_save_buffer; 
    char* file_name;
    char* epc_to_save;
    char* setting_1_config_label;
    char* setting_1_names[2];
    char* setting_2_config_label;
    char* setting_2_entry_text;
    char* setting_2_default_value;
    char* setting_3_config_label;
    char* setting_3_names[2];

    uint32_t temp_buffer_size; 
    uint32_t temp_buffer_save_size; 
    uint32_t name_size;
    uint32_t selected_tag_index;
    uint32_t number_of_saved_tags;
    uint32_t number_of_tids_to_read;
    uint32_t number_of_res_to_read;
    uint32_t number_of_mem_to_read;
    uint32_t cur_epc_index;
    uint32_t cur_tid_index;
    uint32_t cur_res_index;
    uint32_t cur_mem_index;

    UartHelper* uart_helper;

    FuriString* epc_name_delete; 
    FuriString* epc_delete; 
    FuriString* epc_name;
    FuriString* setting_2_power_str;
    FuriString* epc_to_write;

    bool is_reading;
    bool is_writing;
    bool reader_connected;

    FuriTimer* timer; 

    Storage* tag_storage;
    FlipperFormat* epc_file;
    FlipperFormat* epc_index_file;
    
    
    UHFReaderState state;
    
    size_t number_of_epcs_to_read;
    size_t name_size_parse;
    
    uint8_t setting_3_index;
    uint8_t setting_1_index;
    uint8_t setting_1_values[2];
    uint8_t setting_3_values[2];

    char** epc_values; 
    char** tid_values;
    char** res_values;
    char** mem_values;

} UHFReaderApp;

//The model for the configure/read screen
typedef struct {
    uint32_t setting_1_index; 
    FuriString* setting_2_power; 
    uint32_t setting_3_index; 
    bool is_reading;
    FuriString* epc_name;
    uint32_t cur_epc_index;
    FuriString* epc_value;
    uint32_t num_epcs_read;
    FuriString* setting_1_value;
    FuriString* setting_3_value;
    uint32_t scroll_offset;
    char* scrolling_text;
} UHFReaderConfigModel;

//Model for the write screen
typedef struct {
    uint32_t setting_1_index; 
    FuriString* setting_2_power; 
    uint32_t setting_3_index; 
    bool is_writing;
    FuriString* epc_name;
    FuriString* write_function;
    FuriString* epc_value;
    FuriString* write_status;
    FuriString* new_epc_value;
    FuriString* tid_value;
    FuriString* new_tid_value;
    FuriString* res_value;
    FuriString* new_res_value;
    FuriString* mem_value;
    FuriString* new_mem_value;
    FuriString* setting_1_value;
    FuriString* setting_3_value;
} UHFReaderWriteModel;


//Model for the delete screen
typedef struct {
    uint32_t selected_tag_index; 
    FuriString* selected_tag_name; 
    FuriString* selected_tag_epc; 
    uint32_t scroll_offset;
    char* scrolling_text;
} UHFReaderDeleteModel;

//Model use for handling UHF RFID tag data
typedef struct {
    FuriString* Reserved;
    FuriString* EPC; 
    FuriString* TID; 
    FuriString* User; 
    uint32_t cur_epc_index;
    uint32_t scroll_offset_epc;
    char* scrolling_text_epc;
    uint32_t scroll_offset_tid;
    char* scrolling_text_tid;
    uint32_t scroll_offset_res;
    char* scrolling_text_res;
    uint32_t scroll_offset_mem;
    char* scrolling_text_mem;
} UHFRFIDTagModel;