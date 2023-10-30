#pragma once

#include <furi.h>
#include <mjs_core_public.h>
#include <mjs_ffi_public.h>
#include <mjs_exec_public.h>
#include <mjs_object_public.h>
#include <mjs_string_public.h>
#include <mjs_array_public.h>
#include <mjs_util_public.h>
#include <mjs_primitive_public.h>

#define INST_PROP_NAME "_"

#define MFS_MK_FN(fn) mjs_mk_foreign_func(mjs, (mjs_func_ptr_t)fn)

typedef enum {
    ThreadEventStop = (1 << 0),
    ThreadEventCustomDataRx = (1 << 1),
} WorkerEventFlags;

bool js_delay_with_flags(struct mjs* mjs, uint32_t time);

void js_flags_set(struct mjs* mjs, uint32_t flags);

uint32_t js_flags_wait(struct mjs* mjs, uint32_t flags, uint32_t timeout);
