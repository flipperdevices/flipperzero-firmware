#include <stdio.h>
#include "flipper.h"

void furi_test_app(void* param) {
	printf("furi test app\n");

	while(1) {
		delay(1000);
	}
}