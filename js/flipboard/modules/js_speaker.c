#include "../js_modules.h"
#include <furi_hal.h>

typedef struct {
    bool acquired;
} JsSpeakerInst;

static void js_speaker_acquire(struct mjs* mjs) {
    mjs_val_t obj_inst = mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0);
    JsSpeakerInst* speaker = mjs_get_ptr(mjs, obj_inst);
    furi_assert(speaker);

    if(mjs_nargs(mjs) != 1) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "Invalid args (timeoutMs)");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    if(!mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "Invalid numeric arg (timeoutMs)");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    uint32_t timeout = mjs_get_int(mjs, mjs_arg(mjs, 0));

    if(!speaker->acquired) {
        speaker->acquired = furi_hal_speaker_acquire(timeout);
    }

    mjs_return(mjs, mjs_mk_boolean(mjs, speaker->acquired));
}

static void js_speaker_release(struct mjs* mjs) {
    mjs_val_t obj_inst = mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0);
    JsSpeakerInst* speaker = mjs_get_ptr(mjs, obj_inst);
    furi_assert(speaker);

    if(mjs_nargs(mjs) != 0) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "No arguments expected");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    if(speaker->acquired) {
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
        speaker->acquired = false;
    }

    mjs_return(mjs, mjs_mk_boolean(mjs, true));
}

static void js_speaker_start(struct mjs* mjs) {
    mjs_val_t obj_inst = mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0);
    JsSpeakerInst* speaker = mjs_get_ptr(mjs, obj_inst);
    furi_assert(speaker);

    size_t num_args = mjs_nargs(mjs);
    float frequency;
    float volume;

    if(num_args == 1) {
        if(!mjs_is_number(mjs_arg(mjs, 0))) {
            mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "Invalid numeric arg (freq [, volume])");
            mjs_return(mjs, MJS_UNDEFINED);
            return;
        }
        volume = 1.0;
    } else if(num_args == 2) {
        if(!mjs_is_number(mjs_arg(mjs, 0)) || !mjs_is_number(mjs_arg(mjs, 1))) {
            mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "Invalid numeric arg (freq [, volume])");
            mjs_return(mjs, MJS_UNDEFINED);
            return;
        }
        volume = mjs_get_double(mjs, mjs_arg(mjs, 1));
        if(volume < 0 || volume > 1) {
            mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "Invalid volume (0 <= volume <= 1)");
            mjs_return(mjs, MJS_UNDEFINED);
            return;
        }
    } else {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "Invalid args (freq [, volume])");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    if(!speaker->acquired) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "Speaker must be acquired first");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    frequency = mjs_get_double(mjs, mjs_arg(mjs, 0));

    furi_hal_speaker_start(frequency, volume);

    mjs_return(mjs, mjs_mk_boolean(mjs, true));
}

static void js_speaker_stop(struct mjs* mjs) {
    mjs_val_t obj_inst = mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0);
    JsSpeakerInst* speaker = mjs_get_ptr(mjs, obj_inst);
    furi_assert(speaker);

    if(mjs_nargs(mjs) != 0) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "No arguments expected");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    if(!speaker->acquired) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "Speaker must be acquired first");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    furi_hal_speaker_stop();

    mjs_return(mjs, mjs_mk_boolean(mjs, true));
}

static void js_speaker_play(struct mjs* mjs) {
    mjs_val_t obj_inst = mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0);
    JsSpeakerInst* speaker = mjs_get_ptr(mjs, obj_inst);
    furi_assert(speaker);

    size_t num_args = mjs_nargs(mjs);
    float frequency;
    float volume;
    uint32_t duration;
    uint32_t timeout = 1000;
    bool acquired_in_play = false;

    if(num_args != 3) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "Invalid args (freq, volume, duration)");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    if(!mjs_is_number(mjs_arg(mjs, 0)) || !mjs_is_number(mjs_arg(mjs, 1)) ||
       !mjs_is_number(mjs_arg(mjs, 2))) {
        mjs_prepend_errorf(
            mjs, MJS_BAD_ARGS_ERROR, "Invalid numeric arg (freq, volume, duration)");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    frequency = mjs_get_double(mjs, mjs_arg(mjs, 0));
    volume = mjs_get_double(mjs, mjs_arg(mjs, 1));
    duration = mjs_get_int(mjs, mjs_arg(mjs, 2));

    if(!speaker->acquired) {
        acquired_in_play = true;
        speaker->acquired = furi_hal_speaker_acquire(timeout);
    }

    if(speaker->acquired) {
        furi_hal_speaker_start(frequency, volume);
        furi_delay_ms(duration);
        furi_hal_speaker_stop();
        if(acquired_in_play) {
            furi_hal_speaker_release();
            speaker->acquired = false;
        }
        mjs_return(mjs, mjs_mk_boolean(mjs, true));
    } else {
        mjs_return(mjs, mjs_mk_boolean(mjs, false));
    }
}

static void* js_speaker_create(struct mjs* mjs, mjs_val_t* object) {
    JsSpeakerInst* speaker = malloc(sizeof(JsSpeakerInst));
    speaker->acquired = false;
    mjs_val_t speaker_obj = mjs_mk_object(mjs);
    mjs_set(mjs, speaker_obj, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, speaker));
    mjs_set(mjs, speaker_obj, "acquire", ~0, MJS_MK_FN(js_speaker_acquire));
    mjs_set(mjs, speaker_obj, "release", ~0, MJS_MK_FN(js_speaker_release));
    mjs_set(mjs, speaker_obj, "start", ~0, MJS_MK_FN(js_speaker_start));
    mjs_set(mjs, speaker_obj, "stop", ~0, MJS_MK_FN(js_speaker_stop));
    mjs_set(mjs, speaker_obj, "play", ~0, MJS_MK_FN(js_speaker_play));
    *object = speaker_obj;
    return speaker;
}

static void js_speaker_destroy(void* inst) {
    JsSpeakerInst* speaker = (JsSpeakerInst*)inst;
    if(speaker->acquired) {
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
        speaker->acquired = false;
    }
    free(speaker);
}

static const JsModuleDescriptor js_speaker_desc = {
    "speaker",
    js_speaker_create,
    js_speaker_destroy,
};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &js_speaker_desc,
};

const FlipperAppPluginDescriptor* js_speaker_ep(void) {
    return &plugin_descriptor;
}