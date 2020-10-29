#include "flipper_v2.h"

void foo(char* a) {

}

bool init_flipper_api(void) {
	const char* b = NULL;
	foo(b);
	
    return gpio_api_init();
}