#include <furi.h>
#include <furi_hal.h>

void spi_demo();
void spi_demo_v2();

int32_t main_learn_spi(void* _p) {
    UNUSED(_p);
    spi_demo();
    spi_demo_v2();
    return 0;
}