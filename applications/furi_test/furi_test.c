#include <stdio.h>
#include "flipper.h"

void furi_widget(void* param) {
    printf("start furi widget: %s\n", (char*)param);

    while(1) {
        printf("furi widget\n");
        delay(200);
    }
}

void furi_test_app(void* param) {
    printf("start furi test app\n");

    FuriApp widget = furiac_start(furi_widget, "furi_widget", "test widget");

    for(size_t i = 0; i < 10; i++) {
        printf("wait backend\n");
        delay(100);
    }

    if(furiac_kill(widget)) {
        printf("widget killed\n");
    } else {
        printf("kill widget fail\n");
    }

    while(1) {
        printf("furi test app\n");
        delay(500);
    }
}