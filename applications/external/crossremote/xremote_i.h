#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/elements.h>
#include <gui/view.h>
#include <gui/view_stack.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/popup.h>
#include <gui/modules/loading.h>
#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/text_input.h>
#include <gui/modules/button_menu.h>
#include <gui/modules/button_panel.h>
#include <gui/modules/variable_item_list.h>

//#include <lib/subghz/protocols/protocol_items.h> //not found

#include <input/input.h>
#include <notification/notification_messages.h>

#include <string.h>
#include <m-array.h>
#include <toolbox/path.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <core/check.h>
#include <core/common_defines.h>
#include <dolphin/dolphin.h>
#include <infrared.h>
#include <infrared_worker.h>
#include <infrared_transmit.h>
#include <flipper_format/flipper_format.h>
#include "xremote_icons.h"
#include "models/subghz/xremote_sg_remote.h"

#define XREMOTE_APP_FOLDER EXT_PATH("apps_data/xremote")
#define XREMOTE_DEFAULT_REMOTE_NAME "remote"
#define XREMOTE_APP_EXTENSION ".xr"
#define XREMOTE_FILE_TYPE "Cross Remote File"
#define XREMOTE_FILE_VERSION 1
#define XREMOTE_TEXT_STORE_NUM 2
#define XREMOTE_TEXT_STORE_SIZE 128
#define XREMOTE_MAX_ITEM_NAME_LENGTH 22
#define XREMOTE_MAX_REMOTE_NAME_LENGTH 22

#define INFRARED_APP_EXTENSION ".ir"
#define INFRARED_APP_FOLDER ANY_PATH("infrared")

#define SUBGHZ_APP_EXTENSION ".sub"
#define SUBGHZ_APP_FOLDER ANY_PATH("subghz")

#define TAG "XRemote"

typedef enum {
    XRemoteRemoteItemTypeInfrared,
    XRemoteRemoteItemTypeSubGhz,
    XRemoteRemoteItemTypePause,
} XRemoteRemoteItemType;

typedef enum {
    InfraredNotificationMessageSuccess,
    InfraredNotificationMessageGreenOn,
    InfraredNotificationMessageGreenOff,
    InfraredNotificationMessageYellowOn,
    InfraredNotificationMessageYellowOff,
    InfraredNotificationMessageBlinkStartRead,
    InfraredNotificationMessageBlinkStartSend,
    InfraredNotificationMessageBlinkStop,
    PauseNotificationMessageBlinkStartSend,
    PauseNotificationMessageBlinkStop,
    SubGhzNotificationMessageBlinkStartSend,
    SubGhzNotificationMessageBlinkStop,
} XRemoteNotificationMessage;

typedef enum {
    XRemoteTransmittingIdle,
    XRemoteTransmittingStart,
    XRemoteTransmittingStop,
} XRemoteRemoteTransmissionStatus;

struct InfraredSignal {
    bool is_raw;
    union {
        InfraredMessage message;
        InfraredRawSignal raw;
    } payload;
};

struct CrossRemoteItem {
    FuriString* name;
    InfraredSignal* ir_signal;
    SubGhzRemote* sg_signal;
    int16_t type;
    int32_t time;
};

typedef struct CrossRemote CrossRemote;
typedef struct CrossRemoteItem CrossRemoteItem;

typedef struct XRemoteTransmit XRemoteTransmit;
typedef struct XRemotePauseSet XRemotePauseSet;
//typedef struct XRemoteInfoscreen XRemoteInfoscreen;

//typedef struct InfraredRemote InfraredRemote;
//typedef struct InfraredRemoteButton InfraredRemoteButton;
//typedef struct InfraredSignal InfraredSignal;
//typedef struct InfraredRawSignal InfraredRawSignal;