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
    UHFReaderViewEpcDump,
    UHFReaderViewEpcInfo,
    UHFReaderViewDelete,
    UHFReaderViewAbout,
    UHFReaderViewRead,
    UHFReaderViewSaveInput,
    UHFReaderViewRenameInput,
    UHFReaderViewEpcWriteInput,
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
    UHFReaderStateIdle,
    UHFReaderStateWaitForNumber,
    UHFReaderStateCollectEPCs,
    UHFReaderStateDoneCollecting,
    UHFReaderStateWaitForTID,
    UHFReaderStateCollectTIDs,
    UHFReaderStateDoneCollectingTIDs,
    UHFReaderStateWaitForRES,
    UHFReaderStateCollectRESs,
    UHFReaderStateDoneCollectingRESs,
    UHFReaderStateWaitForMEM,
    UHFReaderStateCollectMEMs,
    UHFReaderStateDoneCollectingMEMs
} UHFReaderState;

//The main UHFReaderApp Struct
typedef struct {
    ViewDispatcher* ViewDispatcher;
    NotificationApp* Notifications;

    Submenu* Submenu;
    Submenu* SubmenuSaved;
    Submenu* SubmenuTagActions;

    TextInput* TextInput;
    TextInput* SaveInput;
    TextInput* RenameInput;
    TextInput* EpcWrite;

    VariableItemList* VariableItemListConfig;
    VariableItem* Setting2Item;
    Widget* WidgetAbout;

    View* ViewRead;
    View* ViewWrite;
    View* ViewDelete;
    View* ViewDeleteSuccess;
    View* ViewEpc;
    View* ViewEpcInfo;

    char* TempBuffer;
    char* TempSaveBuffer;
    char* FileName;
    char* EpcToSave;
    char* Setting1ConfigLabel;
    char* Setting1Names[2];
    char* Setting2ConfigLabel;
    char* Setting2EntryText;
    char* Setting2DefaultValue;
    char* Setting3ConfigLabel;
    char* Setting3Names[2];

    uint32_t TempBufferSize;
    uint32_t TempBufferSaveSize;
    uint32_t NameSize;
    uint32_t SelectedTagIndex;
    uint32_t NumberOfSavedTags;
    uint32_t NumberOfTidsToRead;
    uint32_t NumberOfResToRead;
    uint32_t NumberOfMemToRead;
    uint32_t CurEpcIndex;
    uint32_t CurTidIndex;
    uint32_t CurResIndex;
    uint32_t CurMemIndex;

    UartHelper* UartHelper;

    FuriString* EpcNameDelete;
    FuriString* EpcDelete;
    FuriString* EpcName;
    FuriString* Setting2PowerStr;
    FuriString* EpcToWrite;

    bool IsReading;
    bool IsWriting;
    bool ReaderConnected;

    FuriTimer* Timer;

    Storage* TagStorage;
    FlipperFormat* EpcFile;
    FlipperFormat* EpcIndexFile;

    UHFReaderState State;

    size_t NumberOfEpcsToRead;
    size_t NameSizeParse;

    uint8_t Setting3Index;
    uint8_t Setting1Index;
    uint8_t Setting1Values[2];
    uint8_t Setting3Values[2];

    char** EpcValues;
    char** TidValues;
    char** ResValues;
    char** MemValues;

} UHFReaderApp;

//The model for the configure/read screen
typedef struct {
    uint32_t Setting1Index;
    FuriString* Setting2Power;
    uint32_t Setting3Index;
    bool IsReading;
    FuriString* EpcName;
    uint32_t CurEpcIndex;
    FuriString* EpcValue;
    uint32_t NumEpcsRead;
    FuriString* Setting1Value;
    FuriString* Setting3Value;
    uint32_t ScrollOffset;
    char* ScrollingText;
} UHFReaderConfigModel;

//Model for the write screen
typedef struct {
    uint32_t Setting1Index;
    FuriString* Setting2Power;
    uint32_t Setting3Index;
    bool IsWriting;
    FuriString* EpcName;
    FuriString* WriteFunction;
    FuriString* EpcValue;
    FuriString* WriteStatus;
    FuriString* NewEpcValue;
    FuriString* TidValue;
    FuriString* NewTidValue;
    FuriString* ResValue;
    FuriString* NewResValue;
    FuriString* MemValue;
    FuriString* NewMemValue;
    FuriString* Setting1Value;
    FuriString* Setting3Value;
} UHFReaderWriteModel;

//Model for the delete screen
typedef struct {
    uint32_t SelectedTagIndex;
    FuriString* SelectedTagName;
    FuriString* SelectedTagEpc;
    uint32_t ScrollOffset;
    char* ScrollingText;
} UHFReaderDeleteModel;

//Model use for handling UHF RFID tag data
typedef struct {
    FuriString* Reserved;
    FuriString* Epc;
    FuriString* Tid;
    FuriString* User;
    uint32_t CurEpcIndex;
    uint32_t ScrollOffsetEpc;
    char* ScrollingTextEpc;
    uint32_t ScrollOffsetTid;
    char* ScrollingTextTid;
    uint32_t ScrollOffsetRes;
    char* ScrollingTextRes;
    uint32_t ScrollOffsetMem;
    char* ScrollingTextMem;
} UHFRFIDTagModel;
