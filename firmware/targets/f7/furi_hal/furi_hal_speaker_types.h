#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define FURI_HAL_SPEAKER_TIMER TIM16
#define FURI_HAL_SPEAKER_CHANNEL LL_TIM_CHANNEL_CH1
#define FURI_HAL_SPEAKER_PRESCALER 500
#define FURI_HAL_SPEAKER_MAX_VOLUME 60

typedef struct FuriHalSpeakerBus FuriHalSpeakerBus;
typedef struct FuriHalSpeakerBusHandle FuriHalSpeakerBusHandle;

/** FuriHal speaker bus states */
typedef enum {
    FuriHalSpeakerBusEventInit, /**< Bus initialization event, called on system start */
    FuriHalSpeakerBusEventDeinit, /**< Bus deinitialization event, called on system stop */
    FuriHalSpeakerBusEventLock, /**< Bus lock event, called before activation */
    FuriHalSpeakerBusEventUnlock, /**< Bus unlock event, called after deactivation */
    FuriHalSpeakerBusEventActivate, /**< Bus activation event, called before handle activation */
    FuriHalSpeakerBusEventDeactivate, /**< Bus deactivation event, called after handle deactivation  */
} FuriHalSpeakerBusEvent;

/** FuriHal speaker bus event callback */
typedef void (*FuriHalSpeakerBusEventCallback)(FuriHalSpeakerBus* bus, FuriHalSpeakerBusEvent event);

/** FuriHal speaker bus */
struct FuriHalSpeakerBus {
    FuriHalSpeakerBusHandle* current_handle;
    FuriHalSpeakerBusEventCallback callback;
};

/** FuriHal speaker handle states */
typedef enum {
    FuriHalSpeakerBusHandleEventActivate, /**< Handle activate: connect gpio and apply bus config */
    FuriHalSpeakerBusHandleEventDeactivate, /**< Handle deactivate: disconnect gpio and reset bus config */
} FuriHalSpeakerBusHandleEvent;

/** FuriHal speaker handle event callback */
typedef void (*FuriHalSpeakerBusHandleEventCallback)(
    FuriHalSpeakerBusHandle* handle,
    FuriHalSpeakerBusHandleEvent event);

/** FuriHal speaker handle */
struct FuriHalSpeakerBusHandle {
    FuriHalSpeakerBus* bus;
    FuriHalSpeakerBusHandleEventCallback callback;
};

#ifdef __cplusplus
}
#endif
