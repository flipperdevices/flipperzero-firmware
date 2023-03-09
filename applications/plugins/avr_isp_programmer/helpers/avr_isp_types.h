#pragma once

#include <furi.h>
#include <furi_hal.h>

#define AVR_ISP_VERSION_APP "0.1"
#define AVR_ISP_DEVELOPED "SkorP"
#define AVR_ISP_GITHUB "https://github.com/flipperdevices/flipperzero-firmware"

// #define AVR_ISP_KEY_FILE_VERSION 1
// #define AVR_ISP_KEY_FILE_TYPE "Flipper Weather Station Key File"

// /** WSRxKeyState state */
// typedef enum {
//     WSRxKeyStateIDLE,
//     WSRxKeyStateBack,
//     WSRxKeyStateStart,
//     WSRxKeyStateAddKey,
// } WSRxKeyState;

// /** WSHopperState state */
// typedef enum {
//     WSHopperStateOFF,
//     WSHopperStateRunnig,
//     WSHopperStatePause,
//     WSHopperStateRSSITimeOut,
// } WSHopperState;

// /** WSLock */
// typedef enum {
//     WSLockOff,
//     WSLockOn,
// } WSLock;

typedef enum {
    //AvrIspViewVariableItemList,
    AvrIspViewSubmenu,
    AvrIspViewProgrammer,
    AvrIspViewWidget,
} AvrIspView;

// /** AvrIspTxRx state */
// typedef enum {
//     WSTxRxStateIDLE,
//     WSTxRxStateRx,
//     WSTxRxStateTx,
//     WSTxRxStateSleep,
// } WSTxRxState;
