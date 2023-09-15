/*
 * Copyright (c) 2016 Cesanta Software Limited
 * All rights reserved
 */

#include "mjs_conversion.h"
#include "mjs_object.h"
#include "mjs_primitive.h"
#include "mjs_string.h"
#include "mjs_util.h"

MJS_PRIVATE mjs_err_t
    mjs_to_string(struct mjs* mjs, mjs_val_t* v, char** p, size_t* sizep, int* need_free) {
    mjs_err_t ret = MJS_OK;

    *p = NULL;
    *sizep = 0;
    *need_free = 0;

    if(mjs_is_string(*v)) {
        *p = (char*)mjs_get_string(mjs, v, sizep);
    } else if(mjs_is_number(*v)) {
        char buf[50] = "";
        struct json_out out = JSON_OUT_BUF(buf, sizeof(buf));
        mjs_jprintf(*v, mjs, &out);
        *sizep = strlen(buf);
        *p = malloc(*sizep + 1);
        if(*p == NULL) {
            ret = MJS_OUT_OF_MEMORY;
            goto clean;
        }
        memmove(*p, buf, *sizep + 1);
        *need_free = 1;
    } else if(mjs_is_boolean(*v)) {
        if(mjs_get_bool(mjs, *v)) {
            *p = "true";
            *sizep = 4;
        } else {
            *p = "false";
            *sizep = 5;
        }
    } else if(mjs_is_undefined(*v)) {
        *p = "undefined";
        *sizep = 9;
    } else if(mjs_is_null(*v)) {
        *p = "null";
        *sizep = 4;
    } else if(mjs_is_object(*v)) {
        ret = MJS_TYPE_ERROR;
        mjs_set_errorf(mjs, ret, "conversion from object to string is not supported");
    } else if(mjs_is_foreign(*v)) {
        *p = "TODO_foreign";
        *sizep = 12;
    } else {
        ret = MJS_TYPE_ERROR;
        mjs_set_errorf(mjs, ret, "unknown type to convert to string");
    }

clean:
    return ret;
}

MJS_PRIVATE mjs_val_t mjs_to_boolean_v(struct mjs* mjs, mjs_val_t v) {
    size_t len;
    int is_truthy;

    is_truthy = ((mjs_is_boolean(v) && mjs_get_bool(mjs, v)) ||
                 (mjs_is_number(v) && mjs_get_double(mjs, v) != (double)0.0) ||
                 (mjs_is_string(v) && mjs_get_string(mjs, &v, &len) && len > 0) ||
                 (mjs_is_function(v)) || (mjs_is_foreign(v)) || (mjs_is_object(v))) &&
                v != MJS_TAG_NAN;

    return mjs_mk_boolean(mjs, is_truthy);
}

MJS_PRIVATE int mjs_is_truthy(struct mjs* mjs, mjs_val_t v) {
    return mjs_get_bool(mjs, mjs_to_boolean_v(mjs, v));
}
