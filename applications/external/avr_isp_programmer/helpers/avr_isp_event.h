#pragma once

typedef enum {
    //SubmenuIndex
    SubmenuIndexAvrIspProgrammer = 10,
    SubmenuIndexAvrIspReader,
    SubmenuIndexAvrIspWriter,
    SubmenuIndexAvrIspWiring,
    SubmenuIndexAvrIspAbout,
    SubmenuIndexAvrIspReaderTpi,

    //AvrIspCustomEvent
    AvrIspCustomEventSceneChipDetectOk = 100,
    AvrIspCustomEventSceneReadingOk,
    AvrIspCustomEventSceneWritingOk,
    AvrIspCustomEventSceneErrorVerification,
    AvrIspCustomEventSceneErrorReading,
    AvrIspCustomEventSceneErrorWriting,
    AvrIspCustomEventSceneErrorWritingFuse,
    AvrIspCustomEventSceneInputName,
    AvrIspCustomEventSceneSuccess,
    AvrIspCustomEventSceneExit,
    AvrIspCustomEventSceneExitStartMenu,
} AvrIspCustomEvent;
