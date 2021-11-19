//#include "storage-i.h"
#include "furi/pubsub.h"
#include "storage.h"
#include "storage-i.h"
#include "storage/storage-glue.h"

//typedef void (*StorageStatusChangedCallback)(StorageStatus status);
//
//typedef struct {
//    void* context;
//    StorageStatusChangedCallback callback;
//} StorageSubscribeData;
//
//static void storage_pubsub_state_changed_callback(const void* new_state, void* context) {
//    StorageStatusChangedCallback callback = context;
//    callback(*(StorageStatus*)new_state);
//}
//
//FuriPubSubSubscription* storage_subsribe_for_status_change(Storage* storage, StorageStatusChangedCallback callback) {
//    StorageSubscribeData* data = furi_alloc(sizeof(StorageSubscribeData));
//    return furi_pubsub_subscribe(storage->pubsub, storage_pubsub_state_changed_callback, data);
//}
//
//void storage_unsubsribe_for_status_change(Storage* storage, FuriPubSubSubscription* subscription) {
//    furi_pubsub_unsubscribe(storage->pubsub, subscription);
//}

