#include "lf-rfid-debug-app.h"

// app enter function
extern "C" int32_t lf_rfid_debug_app(void* p) {
    LfRfidDebugApp* app = new LfRfidDebugApp();
    app->run();
    delete app;

    return 0;
}
