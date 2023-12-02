#pragma once

typedef enum {
    //TPMSCustomEvent
    TPMSCustomEventStartId = 100,

    TPMSCustomEventSceneSettingLock,

    TPMSCustomEventViewReceiverOK,
    TPMSCustomEventViewReceiverConfig,
    TPMSCustomEventViewReceiverBack,
    TPMSCustomEventViewReceiverOffDisplay,
    TPMSCustomEventViewReceiverUnlock,
} TPMSCustomEvent;
