#include "rfid_debug_app.h"

// app enter function
extern "C" int32_t rfid_debug_app(void* p) {
    UNUSED(p);
    RfidDebugApp* app = new RfidDebugApp();
    app->run();
    delete app;

    return 0;
}
