#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/dialog_ex.h>
#include <storage/storage.h>
#include <stdlib.h>
#include <power/power_service/power.h>
#include "rad_sens_icons.h"
#include <notification/notification.h>

typedef struct RadSensModel RadSensModel;
#define HISTORY_LENGTH 120

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    View* view;
    FuriThread* worker_thread;
    FuriTimer* timer;
    Power* power;
    RadSensModel* model;
} RadSensApp;

struct RadSensModel {
    bool connected;
    bool verified;
    uint32_t dyn_intensity;
    uint32_t stat_intensity;
    uint16_t new_impulse_count;
    uint16_t impulse_count;
    bool vibro_on;
    bool show_history;
    uint32_t dyn_intensity_history[HISTORY_LENGTH];
    uint8_t dyn_intensity_history_index;
    PowerInfo info;
};
