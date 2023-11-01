#include "rolling_flaws_keeloq.h"

#include "rolling_flaws_utils.h"
#include "rolling_flaws_settings.h"

typedef struct {
    uint32_t fix;
    uint32_t hop;
    uint32_t sn;
    uint32_t btn;
    uint32_t cnt;
    uint32_t enc;
    FuriString* mf;
} KeeLoqData;

KeeLoqData* keeloq_data_alloc() {
    KeeLoqData* data = malloc(sizeof(KeeLoqData));
    data->mf = furi_string_alloc();
    return data;
}

void keeloq_data_free(KeeLoqData* data) {
    furi_string_free(data->mf);
    free(data);
}

static uint32_t get_forward_distance(uint32_t current_count, uint32_t new_count) {
    uint32_t distance = 0;
    if(new_count >= current_count) {
        distance = new_count - current_count;
    } else {
        distance = (0xFFFF - current_count) + new_count;
    }

    return distance;
}

static bool is_open(RollingFlawsModel* model, KeeLoqData* data) {
    bool any_mf = rolling_flaws_setting_protocol_mf_name_get(model)[0] == '*';
    if(!any_mf &&
       furi_string_cmp(data->mf, rolling_flaws_setting_protocol_mf_name_get(model)) != 0) {
        FURI_LOG_I(
            TAG,
            "Wrong MF.  Expected >%s< but got >%s<",
            rolling_flaws_setting_protocol_mf_name_get(model),
            furi_string_get_cstr(data->mf));
        furi_string_set(model->status, "BAD MF");
        return false;
    }

    if(data->fix != rolling_flaws_setting_fix_get(model)) {
        FURI_LOG_I(
            TAG,
            "Wrong fix.  Expected >%08lX< but got >%08lX<",
            rolling_flaws_setting_fix_get(model),
            data->fix);
        furi_string_set(model->status, "BAD FIX");
        return false;
    }

    if((rolling_flaws_setting_fix_get(model) & 0xFFFFFFF) == 0) {
        FURI_LOG_I(TAG, "Fix is test. Not checking data.");
        furi_string_set(model->status, "TEST");
        model->future_count = 0xFFFFFFFF;
        model->count = data->cnt;
        return true;
    }

    if(data->enc != FAILED_TO_PARSE) {
        FURI_LOG_I(TAG, "Encrypted payload is %08lX", data->enc);

        if(!rolling_flaws_setting_sn_zero_get(model)) {
            FURI_LOG_I(TAG, "SN wildcard by 00 disabled.");
            if((data->fix & 0xFF) != 0) {
                FURI_LOG_I(TAG, "SN does not end in 00, validating enc %08lX.", data->enc);

                if((data->enc & 0xFF) == 0) {
                    FURI_LOG_I(TAG, "Encrypted payload SN is zero.");
                    furi_string_set(model->status, "SN 00");
                    return false;
                }
            }
        }

        uint8_t match_bits = rolling_flaws_setting_sn_bits_get(model);
        if(match_bits != 0) {
            uint32_t mask = 0xFFFFFFFF;
            mask = mask >> (32 - match_bits);
            uint32_t fix_sn = data->fix & mask;
            uint32_t enc_sn = data->enc & mask;
            if(fix_sn != enc_sn) {
                FURI_LOG_I(TAG, "SN does not match.  Fix: %08lX Enc: %08lX", fix_sn, enc_sn);
                furi_string_set(model->status, "BAD SN");
                return false;
            } else {
                FURI_LOG_I(TAG, "SN matches.  Fix: %08lX Enc: %08lX", fix_sn, enc_sn);
            }
        }
    }

    uint32_t distance = get_forward_distance(model->count, data->cnt);
    FURI_LOG_I(TAG, "Distance: %08lX", distance);
    if(distance == 0 && rolling_flaws_setting_replay_get(model)) {
        FURI_LOG_I(TAG, "Replay attack detected");
        furi_string_set(model->status, "REPLAY");
        model->future_count = 0xFFFFFFFF;
        model->count = data->cnt;
        return true;
    }

    if(rolling_flaws_setting_count_zero_get(model) && data->cnt == 0) {
        FURI_LOG_I(TAG, "Count zero allowed.");
        furi_string_set(model->status, "COUNT0");
        model->future_count = 0xFFFFFFFF;
        // We don't reset count in this case.
        return true;
    }

    if(distance == 0) {
        distance = 0x10000;
    }

    if(distance <= rolling_flaws_setting_window_next_get(model)) {
        FURI_LOG_I(TAG, "Within next window");
        furi_string_set(model->status, "NEXT");
        model->future_count = 0xFFFFFFFF;
        model->count = data->cnt;
        return true;
    }

    if(distance <= rolling_flaws_setting_window_future_get(model)) {
        FURI_LOG_I(TAG, "Within future window");

        if(model->future_count > 0xFFFF) {
            FURI_LOG_I(TAG, "Set future value to %08lX.", data->cnt);
            furi_string_set(model->status, "FUTURE");
            model->future_count = data->cnt;
            return false;
        }

        uint32_t future_gap = get_forward_distance(model->future_count, data->cnt);
        if(future_gap > 0 && future_gap <= rolling_flaws_setting_window_future_gap_get(model)) {
            FURI_LOG_I(TAG, "Future gap accepted. Gap is %08lX", future_gap);
            furi_string_set(model->status, "GAP");
            model->future_count = 0xFFFFFFFF;
            model->count = data->cnt;
            return true;
        }

        if(future_gap == 0) {
            FURI_LOG_I(TAG, "Future gap is zero.  Set future value to %08lX.", data->cnt);
            furi_string_set(model->status, "FUTURE");
            model->future_count = data->cnt;
            return false;
        }

        FURI_LOG_I(
            TAG,
            "Future gap too large.  %08lX > %08lX",
            future_gap,
            rolling_flaws_setting_window_future_gap_get(model));
        furi_string_set(model->status, "BAD GAP");
        model->future_count = data->cnt;
        return false;
    }

    FURI_LOG_I(TAG, "Signal must be from the past (non-future).");
    furi_string_set(model->status, "PAST");
    return false;
}

uint32_t last_decode = 0;
void decode_keeloq(RollingFlawsModel* model, FuriString* buffer, bool sync) {
    FURI_LOG_T(TAG, "Decoding KeeLoq 64bit");
    uint32_t now = furi_get_tick();
    if(now - last_decode < furi_ms_to_ticks(500)) {
        FURI_LOG_D(TAG, "Ignoring decode.  Too soon.");
        last_decode = now;
        return;
    }
    last_decode = now;

    KeeLoqData* data = keeloq_data_alloc();
    __furi_string_extract_string_until(buffer, 0, "MF:", '\r', data->mf);
    __furi_string_extract_string(buffer, 0, "Key:", '\r', model->key);

    data->fix = __furi_string_extract_int(buffer, "Fix:0x", ' ', FAILED_TO_PARSE);
    data->hop = __furi_string_extract_int(buffer, "Hop:0x", ' ', FAILED_TO_PARSE);
    data->sn = __furi_string_extract_int(buffer, "Sn:0x", ' ', FAILED_TO_PARSE);
    if(data->sn == FAILED_TO_PARSE) {
        FURI_LOG_I(TAG, "Sn:0x not found.  Using Fix data.");
        data->sn = data->fix & 0x0FFFFFFF;
    }
    data->btn = __furi_string_extract_int(buffer, "Btn:", '\r', FAILED_TO_PARSE);
    data->cnt = __furi_string_extract_int(buffer, "Cnt:", '\r', FAILED_TO_PARSE);
    // NOTE: "Enc:" needs to be added to "keeloq.c" subghz_protocol_decoder_keeloq_get_string() method.
    data->enc = __furi_string_extract_int(buffer, "Enc:", '\r', FAILED_TO_PARSE);
    FURI_LOG_I(
        TAG,
        "fix: %08lX hop: %08lX sn: %08lX btn: %08lX cnt: %08lX enc:%08lX key:%s mf:%s",
        data->fix,
        data->hop,
        data->sn,
        data->btn,
        data->cnt,
        data->enc,
        furi_string_get_cstr(model->key),
        furi_string_get_cstr(data->mf));

    if(!sync) {
        model->opened = is_open(model, data);
        if(model->opened) {
            model->count = data->cnt;
        }
        __gui_redraw();
    } else {
        model->custom_fix = data->fix;
        model->count = data->cnt;
        model->future_count = 0xFFFFFFFF;
        model->opened = false;
        rolling_flaws_setting_protocol_custom_mf_set(model, data->mf);
        furi_string_set(model->status, "SYNCED");
    }

    keeloq_data_free(data);
}