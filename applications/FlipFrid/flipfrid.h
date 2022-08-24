#pragma once
#include <furi.h>
#include <notification/notification_messages.h>
#include <record_controller.hpp>

class FlipFridApp {
public:
    ~FlipFridApp();
    FlipFridApp();
    void run();
    RecordController<NotificationApp> notification;
};