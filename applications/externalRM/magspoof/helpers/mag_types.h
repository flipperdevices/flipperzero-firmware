#pragma once

#define MAG_VERSION_APP "0.05"
#define MAG_DEVELOPER "Zachary Weiss"
#define MAG_GITHUB "github.com/zacharyweiss/magspoof_flipper"

typedef enum {
    MagViewSubmenu,
    MagViewDialogEx,
    MagViewPopup,
    MagViewLoading,
    MagViewWidget,
    MagViewVariableItemList,
    MagViewTextInput,
    MagViewMagTextInput,
} MagView;

typedef enum {
    MagReverseStateOff,
    MagReverseStateOn,
} MagReverseState;

typedef enum {
    MagTrackStateOneAndTwo,
    MagTrackStateOne,
    MagTrackStateTwo,
    MagTrackStateThree,
} MagTrackState;

typedef enum {
    MagTxStateRFID,
    MagTxStateGPIO,
    MagTxStatePiezo,
    MagTxStateLF_P, // combo of RFID and Piezo
    MagTxStateNFC,
    MagTxCC1101_434,
    MagTxCC1101_868,
} MagTxState;

typedef enum {
    UART_TerminalEventRefreshConsoleOutput = 0,
    UART_TerminalEventStartConsole,
    UART_TerminalEventStartKeyboard,
} UART_TerminalCustomEvent;
