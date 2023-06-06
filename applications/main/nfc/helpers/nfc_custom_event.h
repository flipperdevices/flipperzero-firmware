#pragma once

enum NfcCustomEvent {
    // Reserve first 100 events for button types and indexes, starting from 0
    NfcCustomEventReserved = 100,

    // Mf classic dict attack events
    NfcCustomEventDictAttackComplete,
    NfcCustomEventDictAttackSkip,
    NfcCustomEventDictAttackCardDetected,
    NfcCustomEventDictAttackCardNotDetected,
    NfcCustomEventDictAttackFoundKeyA,
    NfcCustomEventDictAttackFoundKeyB,
    NfcCustomEventDictAttackNewSector,
    NfcCustomEventDictAttackNewKeyBatch,
    NfcCustomEventDictAttackKeyAttackStart,
    NfcCustomEventDictAttackKeyAttackStop,
    NfcCustomEventDictAttackKeyAttackNextSector,

    NfcCustomEventViewExit,
    NfcCustomEventWorkerExit,
    NfcCustomEventWorkerUpdate,
    NfcCustomEventByteInputDone,
    NfcCustomEventTextInputDone,
    NfcCustomEventDictAttackDone,
    NfcCustomEventRpcLoad,
    NfcCustomEventRpcSessionClose,
};
