#pragma once

typedef enum {
    DTMFDolphinEventVolumeUp = 0,
    DTMFDolphinEventVolumeDown,
    DTMFDolphinBlueboxOkCB,
    DTMFDolphinEventStartDialer,
    DTMFDolphinEventStartBluebox,
    DTMFDolphinEventStartRedbox,
    DTMFDolphinEventPlayTones,
    DTMFDolphinEventStopTones,
    DTMFDolphinPlayerEventHalfTransfer,
    DTMFDolphinPlayerEventFullTransfer,
} DTMFDolphinEvent;