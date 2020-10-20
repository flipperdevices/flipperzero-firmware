#include "flipper.h"
#include "irda_nec.h"



void irda(void* p) {
    // TODO use FURI

    while(1) {
        printf("sending ir packet\n");
        // sending ir packet is a time-critical task, so we disable sheduler
        vTaskSuspendAll();
        ir_nec_send(0xF708, 0x59);
        xTaskResumeAll();
        printf("ir packet sended\n");
        delay(1000);
    }
}