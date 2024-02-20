#pragma once

typedef enum {
    EspFlasherEventRefreshConsoleOutput = 0,
    EspFlasherEventStartConsole,
    EspFlasherEventStartKeyboard,
    EspFlasherEventStartFlasher,
    EspFlasherEventRefreshSubmenu
} EspFlasherCustomEvent;
