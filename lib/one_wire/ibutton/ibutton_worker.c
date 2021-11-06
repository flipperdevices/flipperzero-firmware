#include <furi.h>
#include "ibutton_worker.h"
#include "ibutton_key.h"
#include "../one_wire_host.h"

struct iButtonWorker {
    iButtonKey* current_key;
    OneWireHost* host;
};

static int32_t ibutton_worker_thread(void* thread_context) {
    return 0;
}
