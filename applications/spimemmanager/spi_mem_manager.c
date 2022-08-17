#include <furi.h>

#define TAG "SPIMemManager"

int32_t spi_mem_manager_app(void* p) {
    UNUSED(p);
    FURI_LOG_E(TAG, "hello!");
    return 0;
}
