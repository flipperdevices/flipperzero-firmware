#pragma once

typedef enum {
    //SubmenuIndex
    SubmenuIndexAvrIspProgrammer = 10,
    SubmenuIndexAvrIspReader,
    SubmenuIndexAvrIspWriter,
    SubmenuIndexAvrIsWiring,
    SubmenuIndexAvrIspAbout,

    //AvrIspCustomEventStartId = 100,

    //AvrIspCustomEvent
    AvrIspCustomEventSceneChipDetectOk = 100,
    AvrIspCustomEventSceneInputName,
} AvrIspCustomEvent;
