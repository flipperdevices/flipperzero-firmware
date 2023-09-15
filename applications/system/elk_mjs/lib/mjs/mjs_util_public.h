/*
 * Copyright (c) 2016 Cesanta Software Limited
 * All rights reserved
 */

#ifndef MJS_UTIL_PUBLIC_H_
#define MJS_UTIL_PUBLIC_H_

#include "mjs_core_public.h"
#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

const char* mjs_typeof(mjs_val_t v);

void mjs_fprintf(mjs_val_t v, struct mjs* mjs, FILE* fp);
void mjs_sprintf(mjs_val_t v, struct mjs* mjs, char* buf, size_t buflen);

#if MJS_ENABLE_DEBUG

void mjs_disasm(const uint8_t* code, size_t len);
void mjs_dump(struct mjs* mjs, int do_disasm);

#endif

/*
 * Returns the filename corresponding to the given bcode offset.
 */
const char* mjs_get_bcode_filename_by_offset(struct mjs* mjs, int offset);

/*
 * Returns the line number corresponding to the given bcode offset.
 */
int mjs_get_lineno_by_offset(struct mjs* mjs, int offset);

/*
 * Returns bcode offset of the corresponding call frame cf_num, where 0 means
 * the currently executing function, 1 means the first return address, etc.
 *
 * If given cf_num is too large, -1 is returned.
 */
int mjs_get_offset_by_call_frame_num(struct mjs* mjs, int cf_num);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MJS_UTIL_PUBLIC_H_ */
