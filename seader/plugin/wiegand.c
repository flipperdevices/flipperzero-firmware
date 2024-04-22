
#include "interface.h"

#include <lib/bit_lib/bit_lib.h>
#include <flipper_application/flipper_application.h>

/*
 * Huge thanks to the proxmark codebase:
 * https://github.com/RfidResearchGroup/proxmark3/blob/master/client/src/wiegand_formats.c
 */

// Structure for packed wiegand messages
// Always align lowest value (last transmitted) bit to ordinal position 0 (lowest valued bit bottom)
typedef struct {
    uint8_t Length; // Number of encoded bits in wiegand message (excluding headers and preamble)
    uint32_t Top; // Bits in x<<64 positions
    uint32_t Mid; // Bits in x<<32 positions
    uint32_t Bot; // Lowest ordinal positions
} wiegand_message_t;

static inline uint8_t oddparity32(uint32_t x) {
    return bit_lib_test_parity_32(x, BitLibParityOdd);
}

static inline uint8_t evenparity32(uint32_t x) {
    return bit_lib_test_parity_32(x, BitLibParityEven);
}

static int wiegand_C1k35s_parse(uint8_t bit_length, uint64_t bits, FuriString* description) {
    if(bit_length != 35) {
        return 0;
    }

    wiegand_message_t value;
    value.Mid = bits >> 32;
    value.Bot = bits;
    wiegand_message_t* packed = &value;

    uint32_t cn = (packed->Bot >> 1) & 0x000FFFFF;
    uint32_t fc = ((packed->Mid & 1) << 11) | ((packed->Bot >> 21));
    bool valid = (evenparity32((packed->Mid & 0x1) ^ (packed->Bot & 0xB6DB6DB6)) ==
                  ((packed->Mid >> 1) & 1)) &&
                 (oddparity32((packed->Mid & 0x3) ^ (packed->Bot & 0x6DB6DB6C)) ==
                  ((packed->Bot >> 0) & 1)) &&
                 (oddparity32((packed->Mid & 0x3) ^ (packed->Bot & 0xFFFFFFFF)) ==
                  ((packed->Mid >> 2) & 1));

    if(valid) {
        furi_string_cat_printf(description, "C1k35s\nFC: %ld CN: %ld\n", fc, cn);
        return 1;
    }

    return 0;
}

static int wiegand_h10301_parse(uint8_t bit_length, uint64_t bits, FuriString* description) {
    if(bit_length != 26) {
        return 0;
    }

    //E XXXX XXXX XXXX
    //XXXX XXXX XXXX O
    uint32_t eBitMask = 0x02000000;
    uint32_t oBitMask = 0x00000001;
    uint32_t eParityMask = 0x01FFE000;
    uint32_t oParityMask = 0x00001FFE;
    uint8_t eBit = (eBitMask & bits) >> 25;
    uint8_t oBit = (oBitMask & bits) >> 0;

    bool eParity = bit_lib_test_parity_32((bits & eParityMask) >> 13, BitLibParityEven) ==
                   (eBit == 1);
    bool oParity = bit_lib_test_parity_32((bits & oParityMask) >> 1, BitLibParityOdd) ==
                   (oBit == 1);

    FURI_LOG_D(
        PLUGIN_APP_ID,
        "eBit: %d, oBit: %d, eParity: %d, oParity: %d",
        eBit,
        oBit,
        eParity,
        oParity);

    if(eParity && oParity) {
        uint32_t cnMask = 0x1FFFE;
        uint16_t cn = ((bits & cnMask) >> 1);

        uint32_t fcMask = 0x1FE0000;
        uint16_t fc = ((bits & fcMask) >> 17);

        furi_string_cat_printf(description, "H10301\nFC: %d CN: %d\n", fc, cn);
        return 1;
    }

    return 0;
}

static int wiegand_format_count(uint8_t bit_length, uint64_t bits) {
    UNUSED(bit_length);
    UNUSED(bits);
    int count = 0;
    FuriString* ignore = furi_string_alloc();

    count += wiegand_h10301_parse(bit_length, bits, ignore);
    count += wiegand_C1k35s_parse(bit_length, bits, ignore);

    furi_string_free(ignore);

    FURI_LOG_I(PLUGIN_APP_ID, "count: %i", count);
    return count;
}

static void wiegand_format_description(
    uint8_t bit_length,
    uint64_t bits,
    size_t index,
    FuriString* description) {
    FURI_LOG_I(PLUGIN_APP_ID, "description %d", index);
    UNUSED(bit_length);
    UNUSED(bits);

    size_t i = 0;

    i += wiegand_h10301_parse(bit_length, bits, description);
    if(i - 1 == index) {
        return;
    }
    i += wiegand_C1k35s_parse(bit_length, bits, description);
    if(i - 1 == index) {
        return;
    }

    furi_string_cat_printf(description, "[%i] <name> FC: CN:", index);
}

/* Actual implementation of app<>plugin interface */
static const PluginWiegand plugin_wiegand = {
    .name = "Plugin Wiegand",
    .count = &wiegand_format_count,
    .description = &wiegand_format_description,
};

/* Plugin descriptor to comply with basic plugin specification */
static const FlipperAppPluginDescriptor plugin_wiegand_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &plugin_wiegand,
};

/* Plugin entry point - must return a pointer to const descriptor */
const FlipperAppPluginDescriptor* plugin_wiegand_ep(void) {
    return &plugin_wiegand_descriptor;
}
