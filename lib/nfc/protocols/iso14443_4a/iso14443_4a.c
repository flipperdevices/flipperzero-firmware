#include "iso14443_4a.h"

#include <furi.h>

Iso14443_4aData* iso14443_4a_alloc() {
    Iso14443_4aData* data = malloc(sizeof(Iso14443_4aData));
    data->iso14443_3a_data = nfca_alloc();

    return data;
}

void iso14443_4a_free(Iso14443_4aData* data) {
    furi_assert(data);

    nfca_free(data->iso14443_3a_data);
    free(data);
}

void iso14443_4a_reset(Iso14443_4aData* data) {
    furi_assert(data);

    nfca_reset(data->iso14443_3a_data);
}

void iso14443_4a_copy(Iso14443_4aData* data, const Iso14443_4aData* other) {
    furi_assert(data);
    furi_assert(other);

    nfca_copy(data->iso14443_3a_data, other->iso14443_3a_data);
}
