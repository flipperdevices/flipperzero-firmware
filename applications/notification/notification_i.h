#pragma once
#include "notification.h"

#ifdef __cplusplus
extern "C" {
#endif

void notification_internal_message(NotificationApp* app, const NotificationSequence* sequence);

#ifdef __cplusplus
}
#endif