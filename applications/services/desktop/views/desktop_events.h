#pragma once

typedef enum {
    DesktopMainEventLock,
    DesktopMainEventOpenLockMenu,
    DesktopMainEventOpenArchive,
    DesktopMainEventOpenFavoriteDownLong,
    DesktopMainEventOpenFavoriteLeftShort,
    DesktopMainEventOpenFavoriteLeftLong,
    DesktopMainEventOpenFavoriteRightShort,
    DesktopMainEventOpenFavoriteRightLong,
    DesktopMainEventOpenFavoriteUpLong,
    DesktopMainEventOpenMenu,
    DesktopMainEventOpenGamesMenu,
    _DesktopMainEventOpenDebug,
    DesktopMainEventOpenPowerOff,

    DesktopMainEventOpenPassport,
    DesktopMainEventOpenSnake,
    DesktopMainEventOpen2048,
    DesktopMainEventOpenZombiez,
    DesktopMainEventOpenTetris,
    DesktopMainEventOpenDOOM,
    DesktopMainEventOpenJetPack,
    DesktopMainEventOpenClock,
    DesktopDummyEventOpenLeft,
    DesktopDummyEventOpenDown,
    DesktopDummyEventOpenOk,
    DesktopDummyEventOpenUpLong,
    DesktopDummyEventOpenDownLong,
    DesktopDummyEventOpenLeftLong,
    DesktopDummyEventOpenRightLong,
    DesktopDummyEventOpenOkLong,

    DesktopLockedEventOpenPowerOff,
    DesktopLockedEventUnlocked,
    DesktopLockedEventUpdate,
    DesktopLockedEventShowPinInput,
    DesktopLockedEventDoorsClosed,

    DesktopPinInputEventResetWrongPinLabel,
    DesktopPinInputEventUnlocked,
    DesktopPinInputEventUnlockFailed,
    DesktopPinInputEventBack,

    DesktopPinTimeoutExit,

    _DesktopDebugEventDeed,
    _DesktopDebugEventWrongDeed,
    _DesktopDebugEventSaveState,
    _DesktopDebugEventExit,

    DesktopLockMenuEventExit,
    DesktopLockMenuEventLock,
    DesktopLockMenuEventLockShutdown,
    DesktopLockMenuEventWipe,
    DesktopLockMenuEventDummyModeOn,
    DesktopLockMenuEventDummyModeOff,
    DesktopLockMenuEventStealthModeOn,
    DesktopLockMenuEventStealthModeOff,

    DesktopAnimationEventCheckAnimation,
    DesktopAnimationEventNewIdleAnimation,
    DesktopAnimationEventInteractAnimation,

    DesktopSlideshowCompleted,
    DesktopSlideshowPoweroff,

    DesktopHwMismatchExit,

    DesktopEnclaveExit,

    // Global events
    DesktopGlobalBeforeAppStarted,
    DesktopGlobalAfterAppFinished,
    DesktopGlobalAutoLock,
    DesktopGlobalApiUnlock,
} DesktopEvent;
