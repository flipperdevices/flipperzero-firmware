#pragma once

#include "m-string.h"
#include <furi.h>
#include <furi_hal.h>

// /** SubGhzNotification state */
// typedef enum {
//     SubGhzNotificationStateStarting,
//     SubGhzNotificationStateIDLE,
//     SubGhzNotificationStateTx,
//     SubGhzNotificationStateRx,
//     SubGhzNotificationStateRxDone,
// } SubGhzNotificationState;

// /** SubGhzTxRx state */
// typedef enum {
//     SubGhzTxRxStateIDLE,
//     SubGhzTxRxStateRx,
//     SubGhzTxRxStateTx,
//     SubGhzTxRxStateSleep,
// } SubGhzTxRxState;





// /** SubGhzLoadKeyState state */
// typedef enum {
//     SubGhzLoadKeyStateUnknown,
//     SubGhzLoadKeyStateOK,
//     SubGhzLoadKeyStateParseErr,
// } SubGhzLoadKeyState;

/** WSRxKeyState state */
typedef enum {
    WSRxKeyStateIDLE,
    //WSRxKeyStateNoSave,
    //WSRxKeyStateNeedSave,
    WSRxKeyStateBack,
    WSRxKeyStateStart,
    WSRxKeyStateAddKey,
    //WSRxKeyStateExit,
    //WSRxKeyStateRAWLoad,
    //WSRxKeyStateRAWSave,
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
} WeatherStationView;

/** WeatherStationTxRx state */
typedef enum {
    WSTxRxStateIDLE,
    WSTxRxStateRx,
    WSTxRxStateTx,
    WSTxRxStateSleep,
} WSTxRxState;
