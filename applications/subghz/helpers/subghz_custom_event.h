#pragma once

typedef enum {
    SubghzCustomEventSceneDeleteSuccess = 100,
    SubghzCustomEventSceneDelete,
    SubghzCustomEventSceneReceiverInfoTxStart,
    SubghzCustomEventSceneReceiverInfoTxStop,
    SubghzCustomEventSceneReceiverInfoSave,
    SubghzCustomEventSceneSaveName,
    SubghzCustomEventSceneSaveSuccess,
    SubghzCustomEventSceneShowError,
    SubghzCustomEventSceneShowOnlyRX,

    SubghzCustomEventViewReceverOK,
    SubghzCustomEventViewReceverConfig,
    SubghzCustomEventViewReceverBack,

    SubghzCustomEventViewReadRAWBack,
    SubghzCustomEventViewReadRAWIDLE,
    SubghzCustomEventViewReadRAWREC,
    SubghzCustomEventViewReadRAWConfig,
    SubghzCustomEventViewReadRAWMore,

    SubghzCustomEventViewTransmitterBack,
    SubghzCustomEventViewTransmitterSendStart,
    SubghzCustomEventViewTransmitterSendStop,
    SubghzCustomEventViewTransmitterError,
} SubghzCustomEvent;