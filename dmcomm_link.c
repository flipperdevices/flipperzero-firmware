#include "dmcomm_link.h"

int32_t dmcomm_reader(void* context) {
    FURI_LOG_I(TAG, "dmcomm_reader start");
    App* app = context;

    uint32_t loops = 0;
    while(app->dmcomm_run) {
        furi_delay_ms(100);
        loops += 1;
        if(loops > 10)
        {
          loops = 0;
          FURI_LOG_I(TAG, "dmcomm_reader loop");
        }
    }

    FURI_LOG_I(TAG, "dmcomm_reader end");
    return 0;
}
