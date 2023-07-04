#pragma once

void vTaskClearRunTimeStats(TaskHandle_t xHandle) {
#if(configGENERATE_RUN_TIME_STATS == 1)
    xHandle->ulRunTimeCounter = 0;
#endif
}

void vTaskClearTotalRunTimeStats(void) {
#if(configGENERATE_RUN_TIME_STATS == 1)
    ulTaskSwitchedInTime = 0;
    ulTotalRunTime = 0;
#endif
}