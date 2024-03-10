#include "findmy_state.h"
#include <furi_hal.h>

void findmy_startup() {
    if(!furi_hal_is_normal_boot()) return;

    FindMyState state;
    if(findmy_state_load(&state)) {
        findmy_state_apply(&state);
    }
}
