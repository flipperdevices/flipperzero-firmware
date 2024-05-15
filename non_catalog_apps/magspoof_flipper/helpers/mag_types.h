#pragma once

#define MAG_VERSION_APP FAP_VERSION
#define MAG_DEVELOPER "Zachary Weiss"
#define MAG_GITHUB "github.com/zacharyweiss/magspoof_flipper"

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
    MagPinA7,
    MagPinA6,
    MagPinA4,
    MagPinB3,
    MagPinB2,
    MagPinC3,
    MagPinC1,
    MagPinC0,
} MagPin;

#define MAG_STATE_DEFAULT_REVERSE MagReverseStateOff
#define MAG_STATE_DEFAULT_TRACK MagTrackStateOneAndTwo
#define MAG_STATE_DEFAULT_TX MagTxStateGPIO
#define MAG_STATE_DEFAULT_US_CLOCK 240
#define MAG_STATE_DEFAULT_US_INTERPACKET 10
#define MAG_STATE_DEFAULT_PIN_INPUT MagPinA7
#define MAG_STATE_DEFAULT_PIN_OUTPUT MagPinA6
#define MAG_STATE_DEFAULT_PIN_ENABLE MagPinA4
#define MAG_STATE_DEFAULT_ALLOW_UART false
#define MAG_STATE_DEFAULT_N_REPEATS 3
#define MAG_STATE_DEFAULT_REPEAT_MODE true

typedef enum {
    MagViewSubmenu,
    MagViewDialogEx,
    MagViewPopup,
    MagViewLoading,
    MagViewWidget,
    MagViewVariableItemList,
    MagViewTextInput,
    MagViewTextMagInput,
} MagView;

typedef enum {
    UART_TerminalEventRefreshConsoleOutput = 0,
    UART_TerminalEventStartConsole,
    UART_TerminalEventStartKeyboard,
} UART_TerminalCustomEvent;
