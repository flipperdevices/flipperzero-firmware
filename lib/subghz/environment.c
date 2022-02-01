#include "environment.h"

struct SubGhzEnvironment {
    SubGhzKeystore* keystore;
};

SubGhzEnvironment* subghz_environment_alloc() {
    SubGhzEnvironment* instance = furi_alloc(sizeof(SubGhzEnvironment));

    instance->keystore = subghz_keystore_alloc();

    return instance;
}

void subghz_environment_free(SubGhzEnvironment* instance) {
    furi_assert(instance);

    subghz_keystore_free(instance->keystore);

    free(instance);
}

void subghz_environment_load_keystore(SubGhzEnvironment* instance, const char* filename) {
    furi_assert(instance);

    subghz_keystore_load(instance->keystore, filename);
}

SubGhzKeystore*
    subghz_environment_get_keystore(SubGhzEnvironment* instance) {
    furi_assert(instance);

    return instance->keystore;
}
