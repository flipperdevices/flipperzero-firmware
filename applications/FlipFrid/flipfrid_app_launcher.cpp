#include "flipfrid.h"

// app enter function
extern "C" int32_t flipfrid_app(void* p) {
    UNUSED(p);

    FlipFridApp* app = new FlipFridApp();
    app->run();
    delete app;

    return 0;
}
