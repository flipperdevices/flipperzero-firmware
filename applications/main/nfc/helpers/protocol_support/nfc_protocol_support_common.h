#pragma once

typedef enum {
    NfcProtocolFeatureNone = 0,
    NfcProtocolFeatureEmulateUid = 1UL << 0,
    NfcProtocolFeatureEmulateFull = 1UL << 1,
    NfcProtocolFeatureEditUid = 1UL << 2,
} NfcProtocolFeature;

typedef enum {
    NfcProtocolSupportSceneInfo = 0,
    NfcProtocolSupportSceneRead,
    NfcProtocolSupportSceneReadMenu,
    NfcProtocolSupportSceneReadSuccess,
    NfcProtocolSupportSceneSavedMenu,
    NfcProtocolSupportSceneSaveName,
    NfcProtocolSupportSceneEmulate,
    NfcProtocolSupportSceneCardDump,
    NfcProtocolSupportSceneRpc,

    NfcProtocolSupportSceneCount,
} NfcProtocolSupportScene;
