#include "legic_prime_render.h"

static void nfc_render_legic_prime_dcf(const LegicPrimeData* data, FuriString* str) {
    const LegicPrimeTokenType token = legic_prime_get_token_type(data);
    furi_string_cat_printf(
        str,
        "\nDCF: %04X (%s)",
        legic_prime_get_dcf(data),
        token == LegicPrimeTokenTypeNewMedia ? "New media" :
                                               legic_prime_get_token_type_name(token));
}

static void nfc_render_legic_prime_layout(const LegicPrimeData* data, FuriString* str) {
    const LegicPrimeTokenType token = legic_prime_get_token_type(data);
    const uint8_t* d = data->data;

    if(token == LegicPrimeTokenTypeIm) {
        furi_string_cat_printf(
            str,
            "\nUnsegmented\nWRP %u WRC %u RD %u",
            d[7] & 0x0F,
            (d[7] & 0x70) >> 4,
            (d[7] & 0x80) >> 7);
    } else if(token == LegicPrimeTokenTypeImSegmented) {
        LegicPrimeSegment seg = {};
        size_t count = 0;
        while(legic_prime_get_next_segment(data, &seg)) {
            count++;
            furi_string_cat_printf(
                str,
                "\nSeg %u @%03X: %u B%s%s%s",
                (unsigned)count,
                seg.offset,
                seg.length,
                seg.valid ? "" : " invalid",
                seg.rd ? " RD" : "",
                seg.last ? " last" : "");
            furi_string_cat_printf(
                str, "\n  WRP %u WRC %u CRC %s", seg.wrp, seg.wrc, seg.crc_ok ? "OK" : "FAIL");
        }
        if(count == 0) furi_string_cat_str(str, "\nNo valid segment header");
    } else if(token != LegicPrimeTokenTypeNewMedia) {
        // Master token: Proxmark3 `hf legic info` OLE/OL/FL.
        const uint8_t level = d[5] & 0x7F;
        uint8_t ol = 0xFC - d[6];
        uint8_t fl;
        switch(token) {
        case LegicPrimeTokenTypeXam:
            ol = 0x0C - (d[5] >> 4);
            fl = 1;
            break;
        case LegicPrimeTokenTypeIam:
            fl = 0x2F - level + 1;
            break;
        case LegicPrimeTokenTypeSam:
            fl = 0x6F - level + 1;
            break;
        default:
            fl = 0x7F - level + 1;
            break;
        }
        furi_string_cat_printf(
            str, "\nMaster token\nOLE %u OL %u FL %u", (d[5] & 0x80) >> 7, ol, fl);
    }
}

void nfc_render_legic_prime_info(
    const LegicPrimeData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    furi_string_cat_str(str, "UID:");
    for(size_t i = 0; i < LEGIC_PRIME_UID_SIZE; i++) {
        furi_string_cat_printf(str, " %02X", data->data[i]);
    }
    furi_string_cat_printf(
        str, "\nMCC: %02X (%s)", data->data[4], legic_prime_verify_mcc(data) ? "OK" : "FAIL");
    nfc_render_legic_prime_dcf(data, str);

    if(format_type == NfcProtocolFormatTypeFull) {
        furi_string_cat_printf(
            str, "\nMemory: %u bytes", (unsigned)legic_prime_get_mem_size(data->type));
        nfc_render_legic_prime_layout(data, str);
    } else if(legic_prime_get_token_type(data) == LegicPrimeTokenTypeImSegmented) {
        LegicPrimeSegment seg = {};
        size_t count = 0;
        while(legic_prime_get_next_segment(data, &seg)) {
            count++;
        }
        furi_string_cat_printf(str, "\nSegments: %u", (unsigned)count);
    }
}

static void nfc_render_legic_prime_hex(
    const uint8_t* bytes,
    size_t size,
    size_t address,
    uint8_t xor_value,
    FuriString* str) {
    // The font escape applies to one line only.
    for(size_t i = 0; i < size; i++) {
        if((i % 4) == 0) {
            furi_string_cat_printf(
                str, "%s\e*%03X:", furi_string_size(str) ? "\n" : "", (unsigned)(address + i));
        }
        furi_string_cat_printf(str, " %02X", bytes[i] ^ xor_value);
    }
}

void nfc_render_legic_prime_dump(const LegicPrimeData* data, FuriString* str) {
    nfc_render_legic_prime_hex(data->data, legic_prime_get_mem_size(data->type), 0, 0, str);

    // Segment payloads with the MCC obfuscation removed.
    LegicPrimeSegment seg = {};
    for(size_t i = 1; legic_prime_get_next_segment(data, &seg); i++) {
        furi_string_cat_printf(str, "\n\e#Segment %u (decoded)", (unsigned)i);
        nfc_render_legic_prime_hex(
            &data->data[seg.payload_offset],
            seg.payload_length,
            seg.payload_offset,
            data->data[4],
            str);
    }
}
