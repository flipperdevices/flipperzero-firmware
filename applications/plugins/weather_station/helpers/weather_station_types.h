#pragma once

#include <furi.h>
#include <furi_hal.h>

#define WS_KEY_FILE_VERSION 1
#define WS_KEY_FILE_TYPE "Flipper Weather Station Key File"

// /** WSNotification state */
// typedef enum {
//     WSNotificationStateStarting,
//     WSNotificationStateIDLE,
//     WSNotificationStateTx,
//     WSNotificationStateRx,
//     WSNotificationStateRxDone,
// } WSNotificationState;

/** WSRxKeyState state */
typedef enum {
    WSRxKeyStateIDLE,
    //WSRxKeyStateNoSave,
    //WSRxKeyStateNeedSave,
    WSRxKeyStateBack,
    WSRxKeyStateStart,
    WSRxKeyStateAddKey,
    //WSRxKeyStateExit,
} WSRxKeyState;

/** WSHopperState state */
typedef enum {
    WSHopperStateOFF,
    WSHopperStateRunnig,
    WSHopperStatePause,
    WSHopperStateRSSITimeOut,
} WSHopperState;

/** WSLock */
typedef enum {
    WSLockOff,
    WSLockOn,
} WSLock;

typedef enum {
    WeatherStationViewVariableItemList,
    WeatherStationViewSubmenu,
    WeatherStationViewShow,
    WeatherStationViewReceiver,
    WeatherStationViewReceiverInfo,
} WeatherStationView;

/** WeatherStationTxRx state */
typedef enum {
    WSTxRxStateIDLE,
    WSTxRxStateRx,
    WSTxRxStateTx,
    WSTxRxStateSleep,
} WSTxRxState;
