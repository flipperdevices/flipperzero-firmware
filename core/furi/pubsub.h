#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*FuriPubSubCallback)(const void*, void*);

typedef struct FuriPubSub FuriPubSub;

typedef struct FuriPubSubSubscription FuriPubSubSubscription;

/** Allocate and init FuriPubSub
 *
 * Reentrable, Not threadsafe, one owner
 *
 * @return     pointer to FuriPubSub instance
 */
FuriPubSub* furi_pubsub_alloc();

/** Free FuriPubSub
 * 
 * @param      pubsub  FuriPubSub instance
 */
void furi_pubsub_free(FuriPubSub* pubsub);

/** Subscribe to FuriPubSub
 * 
 * Threadsafe, Reentrable
 * 
 * @param      pubsub            pointer to FuriPubSub instance
 * @param[in]  callback          The callback
 * @param      callback_context  The callback context
 *
 * @return     pointer to FuriPubSubSubscription instance
 */
FuriPubSubSubscription* furi_pubsub_subscribe(FuriPubSub* pubsub, FuriPubSubCallback callback, void* callback_context);

/** `furi_pubsub_unsubscribe` to unregister callback.
 *
 * @param      pubsub_subscription  pointer to FuriPubSubSubscription instance
 */
void furi_pubsub_unsubscribe(FuriPubSubSubscription* pubsub_subscription);

/**
 * Use `furi_pubsub_publish` to notify subscribers.
 *
 * @param      pubsub   pointer to FuriPubSub instance
 * @param      message  message pointer to publish
 */
void furi_pubsub_publish(FuriPubSub* pubsub, void* message);

#ifdef __cplusplus
}
#endif
