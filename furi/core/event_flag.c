#include "event_flag.h"
#include "common_defines.h"
#include "check.h"

#include <FreeRTOS.h>
#include <event_groups.h>

#include "event_loop_link_i.h"

#define FURI_EVENT_FLAG_MAX_BITS_EVENT_GROUPS 24U
#define FURI_EVENT_FLAG_VALID_BITS            ((1UL << FURI_EVENT_FLAG_MAX_BITS_EVENT_GROUPS) - 1U)
#define FURI_EVENT_FLAG_INVALID_BITS          (~(FURI_EVENT_FLAG_VALID_BITS))

struct FuriEventFlag {
    StaticEventGroup_t container;
    FuriEventLoopLink event_loop_link;
};

// IMPORTANT: container MUST be the FIRST struct member
static_assert(offsetof(FuriEventFlag, container) == 0);

FuriEventFlag* furi_event_flag_alloc(void) {
    furi_check(!FURI_IS_IRQ_MODE());

    FuriEventFlag* instance = malloc(sizeof(FuriEventFlag));

    furi_check(xEventGroupCreateStatic(&instance->container) == (EventGroupHandle_t)instance);

    return instance;
}

void furi_event_flag_free(FuriEventFlag* instance) {
    furi_check(!FURI_IS_IRQ_MODE());

    // Event Loop must be disconnected
    furi_check(!instance->event_loop_link.item_in);
    furi_check(!instance->event_loop_link.item_out);

    vEventGroupDelete((EventGroupHandle_t)instance);
    free(instance);
}

uint32_t furi_event_flag_set(FuriEventFlag* instance, uint32_t flags) {
    furi_check(instance);
    furi_check((flags & FURI_EVENT_FLAG_INVALID_BITS) == 0U);

    EventGroupHandle_t hEventGroup = (EventGroupHandle_t)instance;
    uint32_t rflags;
    BaseType_t yield;

    FURI_CRITICAL_ENTER();

    if(FURI_IS_IRQ_MODE()) {
        yield = pdFALSE;
        if(xEventGroupSetBitsFromISR(hEventGroup, (EventBits_t)flags, &yield) == pdFAIL) {
            rflags = (uint32_t)FuriFlagErrorResource;
        } else {
            rflags = flags;
            portYIELD_FROM_ISR(yield);
        }
    } else {
        rflags = xEventGroupSetBits(hEventGroup, (EventBits_t)flags);
    }

    if(rflags & flags) {
        furi_event_loop_link_notify(&instance->event_loop_link, FuriEventLoopEventIn);
    }

    FURI_CRITICAL_EXIT();

    /* Return event flags after setting */
    return rflags;
}

uint32_t furi_event_flag_clear(FuriEventFlag* instance, uint32_t flags) {
    furi_check(instance);
    furi_check((flags & FURI_EVENT_FLAG_INVALID_BITS) == 0U);

    EventGroupHandle_t hEventGroup = (EventGroupHandle_t)instance;
    uint32_t rflags;

    FURI_CRITICAL_ENTER();
    if(FURI_IS_IRQ_MODE()) {
        rflags = xEventGroupGetBitsFromISR(hEventGroup);

        if(xEventGroupClearBitsFromISR(hEventGroup, (EventBits_t)flags) == pdFAIL) {
            rflags = (uint32_t)FuriStatusErrorResource;
        } else {
            /* xEventGroupClearBitsFromISR only registers clear operation in the timer command queue. */
            /* Yield is required here otherwise clear operation might not execute in the right order. */
            /* See https://github.com/FreeRTOS/FreeRTOS-Kernel/issues/93 for more info.               */
            portYIELD_FROM_ISR(pdTRUE);
        }
    } else {
        rflags = xEventGroupClearBits(hEventGroup, (EventBits_t)flags);
    }

    if(rflags & flags) {
        furi_event_loop_link_notify(&instance->event_loop_link, FuriEventLoopEventOut);
    }
    FURI_CRITICAL_EXIT();

    /* Return event flags before clearing */
    return rflags;
}

uint32_t furi_event_flag_get(FuriEventFlag* instance) {
    furi_check(instance);

    EventGroupHandle_t hEventGroup = (EventGroupHandle_t)instance;
    uint32_t rflags;

    if(FURI_IS_IRQ_MODE()) {
        rflags = xEventGroupGetBitsFromISR(hEventGroup);
    } else {
        rflags = xEventGroupGetBits(hEventGroup);
    }

    /* Return current event flags */
    return rflags;
}

uint32_t furi_event_flag_wait(
    FuriEventFlag* instance,
    uint32_t flags,
    uint32_t options,
    uint32_t timeout) {
    furi_check(!FURI_IS_IRQ_MODE());
    furi_check(instance);
    furi_check((flags & FURI_EVENT_FLAG_INVALID_BITS) == 0U);

    EventGroupHandle_t hEventGroup = (EventGroupHandle_t)instance;
    BaseType_t wait_all;
    BaseType_t exit_clr;
    uint32_t rflags;

    if(options & FuriFlagWaitAll) {
        wait_all = pdTRUE;
    } else {
        wait_all = pdFAIL;
    }

    if(options & FuriFlagNoClear) {
        exit_clr = pdFAIL;
    } else {
        exit_clr = pdTRUE;
    }

    rflags = xEventGroupWaitBits(
        hEventGroup, (EventBits_t)flags, exit_clr, wait_all, (TickType_t)timeout);

    if(options & FuriFlagWaitAll) {
        if((flags & rflags) != flags) {
            if(timeout > 0U) {
                rflags = (uint32_t)FuriStatusErrorTimeout;
            } else {
                rflags = (uint32_t)FuriStatusErrorResource;
            }
        }
    } else {
        if((flags & rflags) == 0U) {
            if(timeout > 0U) {
                rflags = (uint32_t)FuriStatusErrorTimeout;
            } else {
                rflags = (uint32_t)FuriStatusErrorResource;
            }
        }
    }

    if((rflags & FuriFlagError) == 0U) {
        furi_event_loop_link_notify(&instance->event_loop_link, FuriEventLoopEventOut);
    }

    /* Return event flags before clearing */
    return rflags;
}

static FuriEventLoopLink* furi_event_flag_event_loop_get_link(FuriEventLoopObject* object) {
    FuriEventFlag* instance = object;
    furi_assert(instance);
    return &instance->event_loop_link;
}

static bool
    furi_event_flag_event_loop_get_level(FuriEventLoopObject* object, FuriEventLoopEvent event) {
    FuriEventFlag* instance = object;
    furi_assert(instance);

    if(event == FuriEventLoopEventIn) {
        return (furi_event_flag_get(instance) & FURI_EVENT_FLAG_VALID_BITS);
    } else if(event == FuriEventLoopEventOut) {
        return (furi_event_flag_get(instance) & FURI_EVENT_FLAG_VALID_BITS) !=
               FURI_EVENT_FLAG_VALID_BITS;
    } else {
        furi_crash();
    }
}

const FuriEventLoopContract furi_event_flag_event_loop_contract = {
    .get_link = furi_event_flag_event_loop_get_link,
    .get_level = furi_event_flag_event_loop_get_level,
};
