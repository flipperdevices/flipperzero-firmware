#include "ibutton-app.h"

// app enter function
extern "C" void app_ibutton(void* p) {
    iButtonApp* app = new iButtonApp();
    app->run();
    delete app;

    furiac_exit(NULL);
}