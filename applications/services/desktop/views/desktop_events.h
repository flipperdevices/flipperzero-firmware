#pragma once

typedef enum {
    DesktopMainEventOpenLockMenu,
    DesktopMainEventOpenArchive,
    DesktopMainEventOpenFavoritePrimary,
    DesktopMainEventOpenFavoriteSecondary,
    DesktopMainEventOpenFavoriteTertiary,
    DesktopMainEventOpenFavoriteQuaternary,
    DesktopMainEventOpenMenu,
    DesktopMainEventOpenGamesMenu,
    DesktopMainEventOpenDebug,
    DesktopMainEventOpenPassport,
    DesktopMainEventOpenPowerOff,
    DesktopMainEventLock,

    DesktopMainEventOpenSnake,
    DesktopMainEventOpen2048,
    DesktopMainEventOpenZombiez,
    DesktopMainEventOpenTetris,
    DesktopMainEventOpenDOOM,
    DesktopMainEventOpenHeap,
    DesktopMainEventOpenJetPack,
    DesktopMainEventOpenClock,

    DesktopLockedEventOpenPowerOff,
    DesktopLockedEventUnlocked,
    DesktopLockedEventUpdate,
    DesktopLockedEventShowPinInput,

    DesktopPinInputEventResetWrongPinLabel,
    DesktopPinInputEventUnlocked,
    DesktopPinInputEventUnlockFailed,
    DesktopPinInputEventBack,

    DesktopPinTimeoutExit,

    DesktopDebugEventDeed,
    DesktopDebugEventWrongDeed,
    DesktopDebugEventSaveState,
    DesktopDebugEventExit,

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

    // Global events
    DesktopGlobalBeforeAppStarted,
    DesktopGlobalAfterAppFinished,
    DesktopGlobalAutoLock,
} DesktopEvent;
