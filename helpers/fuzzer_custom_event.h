#pragma once

typedef enum {

    // FuzzerCustomEvent
    FuzzerCustomEventViewMainBack = 100,
    FuzzerCustomEventViewMainOk,
    FuzzerCustomEventViewMainPopupErr,

    FuzzerCustomEventViewAttackEnd,

    FuzzerCustomEventViewAttackExit,
    FuzzerCustomEventViewAttackRunAttack,
    FuzzerCustomEventViewAttackPause,
    FuzzerCustomEventViewAttackIdle, // Setup
    FuzzerCustomEventViewAttackEmulateCurrent,
    FuzzerCustomEventViewAttackSave,
    FuzzerCustomEventViewAttackNextUid,
    FuzzerCustomEventViewAttackPrevUid,

    FuzzerCustomEventViewFieldEditorBack,
    FuzzerCustomEventViewFieldEditorOk,

    FuzzerCustomEventTextEditResult,

    FuzzerCustomEventPopupClosed,
} FuzzerCustomEvent;