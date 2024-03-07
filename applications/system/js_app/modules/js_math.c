#include "../js_modules.h"
#include "furi_hal_random.h"

#define MJS_PI (double)3.14159265358979323846
#define MJS_E (double)2.7182818284590452354

static void ret_bad_args(struct mjs* mjs, const char* error) {
    mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "%s", error);
    mjs_return(mjs, mjs_mk_undefined());
}

static bool check_arg_count(struct mjs* mjs, size_t count) {
    size_t num_args = mjs_nargs(mjs);
    if(num_args != count) {
        ret_bad_args(mjs, "Wrong argument count");
        return false;
    }
    return true;
}

void mjs_abs(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    mjs_return(mjs, x < 0 ? mjs_mk_number(mjs, -x) : mjs_arg(mjs, 0));
}

void mjs_acos(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    if (x < -1 || x > 1) {
        ret_bad_args(mjs, "Invalid input value for Math.acos");
        mjs_return(mjs, MJS_UNDEFINED);
    }
    mjs_return(mjs, mjs_mk_number(mjs, MJS_PI / (double)2 - atan(x / sqrt(1 - x * x))));
}

void mjs_acosh(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    if (x < 1) {
        ret_bad_args(mjs, "Invalid input value for Math.acosh");
        mjs_return(mjs, MJS_UNDEFINED);
    }
    mjs_return(mjs, mjs_mk_number(mjs, log(x + sqrt(x * x - 1))));
}

void mjs_asin(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    mjs_return(mjs, mjs_mk_number(mjs, atan(x / sqrt(1 - x * x))));
}

void mjs_asinh(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    mjs_return(mjs, mjs_mk_number(mjs, log(x + sqrt(x * x + 1))));
}

void mjs_atan(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    mjs_return(mjs, mjs_mk_number(mjs, atan(x)));
}

void mjs_atan2(struct mjs* mjs) {
    if (!check_arg_count(mjs, 2) || !mjs_is_number(mjs_arg(mjs, 0)) || !mjs_is_number(mjs_arg(mjs, 1))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double y = mjs_get_double(mjs, mjs_arg(mjs, 0));
    double x = mjs_get_double(mjs, mjs_arg(mjs, 1));
    mjs_return(mjs, mjs_mk_number(mjs, atan2(y, x)));
}

void mjs_atanh(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    if (x <= -1 || x >= 1) {
        ret_bad_args(mjs, "Invalid input value for Math.atanh");
        mjs_return(mjs, MJS_UNDEFINED);
    }
    mjs_return(mjs, mjs_mk_number(mjs, (double)0.5 * log((1 + x) / (1 - x))));
}

void mjs_cbrt(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    mjs_return(mjs, mjs_mk_number(mjs, pow(x, 1.0 / 3.0)));
}

void mjs_ceil(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    mjs_return(mjs, mjs_mk_number(mjs, (int)(x + (double)0.5)));
}

void mjs_clz32(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    unsigned int x = (unsigned int)mjs_get_int(mjs, mjs_arg(mjs, 0));
    int count = 0;
    while (x) {
        x >>= 1;
        count++;
    }
    mjs_return(mjs, mjs_mk_number(mjs, 32 - count));
}

void mjs_cos(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    mjs_return(mjs, mjs_mk_number(mjs, cos(x)));
}

void mjs_exp(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    double result = 1;
    double term = 1;
    for (int i = 1; i < 100; i++) {
        term *= x / i;
        result += term;
    }
    mjs_return(mjs, mjs_mk_number(mjs, result));
}

void mjs_floor(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    mjs_return(mjs, mjs_mk_number(mjs, (int)x));
}

void mjs_log(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    if (x <= 0) {
        ret_bad_args(mjs, "Invalid input value for Math.log");
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double result = 0;
    while (x >= MJS_E) {
        x /= MJS_E;
        result++;
    }
    mjs_return(mjs, mjs_mk_number(mjs, result + log(x)));
}

void mjs_max(struct mjs* mjs) {
    if (!check_arg_count(mjs, 2) || !mjs_is_number(mjs_arg(mjs, 0)) || !mjs_is_number(mjs_arg(mjs, 1))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    double y = mjs_get_double(mjs, mjs_arg(mjs, 1));
    mjs_return(mjs, mjs_mk_number(mjs, x > y ? x : y));
}

void mjs_min(struct mjs* mjs) {
    if (!check_arg_count(mjs, 2) || !mjs_is_number(mjs_arg(mjs, 0)) || !mjs_is_number(mjs_arg(mjs, 1))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    double y = mjs_get_double(mjs, mjs_arg(mjs, 1));
    mjs_return(mjs, mjs_mk_number(mjs, x < y ? x : y));
}

void mjs_pow(struct mjs* mjs) {
    if (!check_arg_count(mjs, 2) || !mjs_is_number(mjs_arg(mjs, 0)) || !mjs_is_number(mjs_arg(mjs, 1))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double base = mjs_get_double(mjs, mjs_arg(mjs, 0));
    double exponent = mjs_get_double(mjs, mjs_arg(mjs, 1));
    double result = 1;
    for (int i = 0; i < exponent; i++) {
        result *= base;
    }
    mjs_return(mjs, mjs_mk_number(mjs, result));
}

void mjs_random(struct mjs* mjs) {
    if (!check_arg_count(mjs, 0)) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    const uint32_t random_val = furi_hal_random_get();
    double rnd = (double)random_val / RAND_MAX;
    mjs_return(mjs, mjs_mk_number(mjs, rnd));
}

void mjs_sign(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    mjs_return(mjs, mjs_mk_number(mjs, x == 0 ? 0 : (x < 0 ? -1 : 1)));
}

void mjs_sin(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    double result = x;
    double term = x;
    for (int i = 1; i < 10; i++) {
        term *= -x * x / ((2 * i) * (2 * i + 1));
        result += term;
    }
    mjs_return(mjs, mjs_mk_number(mjs, result));
}

void mjs_sqrt(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    if (x < 0) {
        ret_bad_args(mjs, "Invalid input value for Math.sqrt");
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double result = 1;
    while (result * result < x) {
        result += (double)0.001;
    }
    mjs_return(mjs, mjs_mk_number(mjs, result));
}

void mjs_trunc(struct mjs* mjs) {
    if (!check_arg_count(mjs, 1) || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_return(mjs, MJS_UNDEFINED);
    }
    double x = mjs_get_double(mjs, mjs_arg(mjs, 0));
    mjs_return(mjs, mjs_mk_number(mjs, x < 0 ? ceil(x) : floor(x)));
}


static void* js_math_create(struct mjs *mjs, mjs_val_t* object) {
    mjs_val_t math_obj = mjs_mk_object(mjs);
    mjs_set(mjs, math_obj, "abs", ~0, MJS_MK_FN(mjs_abs));
    mjs_set(mjs, math_obj, "acos", ~0, MJS_MK_FN(mjs_acos));
    mjs_set(mjs, math_obj, "acosh", ~0, MJS_MK_FN(mjs_acosh));
    mjs_set(mjs, math_obj, "asin", ~0, MJS_MK_FN(mjs_asin));
    mjs_set(mjs, math_obj, "asinh", ~0, MJS_MK_FN(mjs_asinh));
    mjs_set(mjs, math_obj, "atan", ~0, MJS_MK_FN(mjs_atan));
    mjs_set(mjs, math_obj, "atan2", ~0, MJS_MK_FN(mjs_atan2));
    mjs_set(mjs, math_obj, "atanh", ~0, MJS_MK_FN(mjs_atanh));
    mjs_set(mjs, math_obj, "cbrt", ~0, MJS_MK_FN(mjs_cbrt));
    mjs_set(mjs, math_obj, "ceil", ~0, MJS_MK_FN(mjs_ceil));
    mjs_set(mjs, math_obj, "clz32", ~0, MJS_MK_FN(mjs_clz32));
    mjs_set(mjs, math_obj, "cos", ~0, MJS_MK_FN(mjs_cos));
    mjs_set(mjs, math_obj, "exp", ~0, MJS_MK_FN(mjs_exp));
    mjs_set(mjs, math_obj, "floor", ~0, MJS_MK_FN(mjs_floor));
    mjs_set(mjs, math_obj, "log", ~0, MJS_MK_FN(mjs_log));
    mjs_set(mjs, math_obj, "max", ~0, MJS_MK_FN(mjs_max));
    mjs_set(mjs, math_obj, "min", ~0, MJS_MK_FN(mjs_min));
    mjs_set(mjs, math_obj, "pow", ~0, MJS_MK_FN(mjs_pow));
    mjs_set(mjs, math_obj, "random", ~0, MJS_MK_FN(mjs_random));
    mjs_set(mjs, math_obj, "sign", ~0, MJS_MK_FN(mjs_sign));
    mjs_set(mjs, math_obj, "sin", ~0, MJS_MK_FN(mjs_sin));
    mjs_set(mjs, math_obj, "sqrt", ~0, MJS_MK_FN(mjs_sqrt));
    mjs_set(mjs, math_obj, "trunc", ~0, MJS_MK_FN(mjs_trunc));
    mjs_set(mjs, math_obj, "PI", ~0, mjs_mk_number(mjs, MJS_PI));
    *object = math_obj;
    return object;
}   

static void js_math_destroy(void *ptr) {
    UNUSED(ptr);
}

static const JsModuleDescriptor js_math_desc = {
    "math",
    js_math_create,
    js_math_destroy,
};

static const FlipperAppPluginDescriptor btkicker_plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &js_math_desc,
};

const FlipperAppPluginDescriptor* js_math_ep(void) {
    return &btkicker_plugin_descriptor;
}