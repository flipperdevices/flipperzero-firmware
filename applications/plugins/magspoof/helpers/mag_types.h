#pragma once

#define MAG_VERSION_APP "0.01"
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
    MagReverseStateOn,
    MagReverseStateOff,
} MagReverseState;

typedef enum {
    MagTrackStateAll,
    MagTrackStateOne,
    MagTrackStateTwo,
} MagTrackState;

typedef enum {
    MagTxStateRFID,
    MagTxStateGPIOA6A7,
} MagTxState;
