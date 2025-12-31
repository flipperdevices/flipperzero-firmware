#include <assert.h>
#include <furi.h>
#include <furi_hal.h>
#include <stdint.h>
#include <storage/storage.h>
#include "keyboard.h"
#include "keyboard_i.h"

struct Header {
    struct SharedHeader shared;
    uint16_t codepoint_range_len;
    uint16_t key_sequences_len;
} FURI_PACKED;

static_assert(sizeof(struct Header) % sizeof(uint32_t) == 0);

#define CODEPOINTS_IN_CODEPOINT_RANGE 0x100

struct CodepointRange {
    uint32_t range;
    uint16_t key_offsets[CODEPOINTS_IN_CODEPOINT_RANGE];
} FURI_PACKED;

static_assert(sizeof(struct CodepointRange) % sizeof(uint32_t) == 0);

struct Layout {
    struct CodepointRange* codepoint_range;
    size_t codepoint_range_len;

    uint16_t* key_sequences;
    size_t key_sequences_len;
};

#define CODEPOINT_LOCAL_MASK 0x000000ff
#define CODEPOINT_RANGE_MASK 0xffffff00

static_assert(CODEPOINT_LOCAL_MASK < CODEPOINTS_IN_CODEPOINT_RANGE);

static struct CodepointRange*
    get_codepoint_range(struct Layout* layout, FuriStringUnicodeValue codepoint) {
    uint32_t wanted_codepoint_range = codepoint & CODEPOINT_RANGE_MASK;

    for(size_t i = 0; i < layout->codepoint_range_len; i++) {
        if(layout->codepoint_range[i].range == wanted_codepoint_range) {
            return &layout->codepoint_range[i];
        }
    }

    return NULL;
}

static size_t key_sequence_len(uint16_t* keycode_sequence) {
    size_t i = 0;
    while(keycode_sequence[i] != 0) {
        i++;
    }
    return i;
}

static uint16_t* get_key_sequence(void* data, FuriStringUnicodeValue codepoint) {
    struct Layout* layout = data;

    struct CodepointRange* codepoint_range = get_codepoint_range(layout, codepoint);
    if(codepoint_range == NULL) {
        return NULL;
    }

    uint32_t codepoint_local = codepoint & CODEPOINT_LOCAL_MASK;
    uint16_t offset = codepoint_range->key_offsets[codepoint_local];

    if(offset >= layout->key_sequences_len) {
        return NULL;
    }

    return &layout->key_sequences[offset];
}

static uint16_t get_single_key(void* data, FuriStringUnicodeValue codepoint) {
    struct Layout* layout = data;

    uint16_t* keycode_sequence = get_key_sequence(layout, codepoint);
    if(key_sequence_len(keycode_sequence) != 1) {
        return 0;
    }

    return keycode_sequence[0];
}

static void free_layout(void* data) {
    struct Layout* layout = data;

    free(layout->key_sequences);
    free(layout->codepoint_range);
    free(layout);
}

static const struct BadUsbKeyboardVTable vtable = {
    .get_key = get_single_key,
    .get_key_sequence = get_key_sequence,
    .free = free_layout,
};

static bool is_layout_valid(struct Layout* layout) {
    for(size_t i = 0; i < layout->codepoint_range_len; i++) {
        struct CodepointRange* codepoint_range = &layout->codepoint_range[i];
        if((codepoint_range->range & CODEPOINT_LOCAL_MASK) != 0) {
            return false;
        }

        for(size_t j = 0; j < CODEPOINTS_IN_CODEPOINT_RANGE; j++) {
            if(codepoint_range->key_offsets[j] >= layout->key_sequences_len) {
                return false;
            }
        }
    }

    // Safety check to prevent us trying to read off the edge
    if(layout->key_sequences[layout->key_sequences_len - 1] != 0x0000) {
        return false;
    }

    return true;
}

BadUsbKeyboard* bad_usb_keyboard_v2_alloc_read(File* layout_file) {
    struct Header header;
    if(storage_file_read(layout_file, &header, sizeof(header)) != sizeof(header)) {
        return NULL;
    }

    struct Layout* layout = malloc(sizeof(*layout));

    size_t codepoint_range_size = header.codepoint_range_len * sizeof(*layout->codepoint_range);
    layout->codepoint_range = malloc(codepoint_range_size);
    layout->codepoint_range_len = header.codepoint_range_len;

    size_t key_sequences_size = header.key_sequences_len * sizeof(*layout->key_sequences);
    layout->key_sequences = malloc(key_sequences_size);
    layout->key_sequences_len = header.key_sequences_len;

    if(storage_file_read(layout_file, layout->codepoint_range, codepoint_range_size) !=
       codepoint_range_size) {
        free(layout->key_sequences);
        free(layout->codepoint_range);
        free(layout);
        return NULL;
    }

    if(storage_file_read(layout_file, layout->key_sequences, key_sequences_size) !=
       key_sequences_size) {
        free(layout->key_sequences);
        free(layout->codepoint_range);
        free(layout);
        return NULL;
    }

    if(!is_layout_valid(layout)) {
        free(layout->key_sequences);
        free(layout->codepoint_range);
        free(layout);
        return NULL;
    }

    BadUsbKeyboard* keyboard = malloc(sizeof(BadUsbKeyboard));
    keyboard->data = layout;
    keyboard->vtable = &vtable;

    return keyboard;
}
