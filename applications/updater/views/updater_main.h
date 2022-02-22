#pragma once

#include <gui/view.h>
//#include "updater_events.h"

typedef struct UpdaterMainView UpdaterMainView;
typedef struct FuriPubSubSubscription FuriPubSubSubscription;

//typedef void (*UpdaterMainViewCallback)(UpdaterEvent event, void* context);

//void updater_main_set_callback(
//    UpdaterMainView* main_view,
//    UpdaterMainViewCallback callback,
//    void* context);

View* updater_main_get_view(UpdaterMainView* main_view);

UpdaterMainView* updater_main_alloc();

void updater_main_free(UpdaterMainView* main_view);

void updater_main_model_set_state(UpdaterMainView* main_view, const char* message, uint8_t progress);

void updater_main_set_storage_pubsub(UpdaterMainView* main_view, FuriPubSubSubscription* sub);

FuriPubSubSubscription* updater_main_get_storage_pubsub(UpdaterMainView* main_view);
