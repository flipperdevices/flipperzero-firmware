#pragma once

#define MAG_VERSION_APP "0.06"
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
} MagView;

typedef enum {
    MagSettingPinA7,
    MagSettingPinA6,
    MagSettingPinA4,
    MagSettingPinB3,
    MagSettingPinB2,
    MagSettingPinC3,
    MagSettingPinC1,
    MagSettingPinC0,
} MagSettingPin;

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
