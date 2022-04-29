#include "ibutton_app.h"

// app enter function
extern "C" int32_t ibutton_app_old(void* p) {
    iButtonApp* app = new iButtonApp();
    app->run(p);
    delete app;

    return 255;
}
