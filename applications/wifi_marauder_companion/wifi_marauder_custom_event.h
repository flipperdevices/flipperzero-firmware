#pragma once

typedef enum {
    WifiMarauderStartEventScan = 0,
    WifiMarauderStartEventAttack,
    WifiMarauderStartEventSniff,
    WifiMarauderStartEventUpdate,
    WifiMarauderStartEventReboot,
    WifiMarauderStartEventChannel,
} WifiMarauderCustomEvent;
