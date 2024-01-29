#include "slix_unlock.h"

#include <furi/furi.h>

SlixUnlock* slix_unlock_alloc() {
    SlixUnlock* instance = malloc(sizeof(SlixUnlock));

    return instance;
}

void slix_unlock_free(SlixUnlock* instance) {
    furi_assert(instance);

    free(instance);
}

void slix_unlock_reset(SlixUnlock* instance) {
    furi_assert(instance);

    memset(instance, 0, sizeof(SlixUnlock));
}
