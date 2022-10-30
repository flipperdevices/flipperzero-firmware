#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include "lightmeter_icons.h"
#include "../lightmeterI2C.h"

#define APP_NAME "Light Meter"

// Main screen
typedef enum {
    FIXED_TIME,
    FIXED_APERTURE,

    /* Know menu Size*/
    MODES_SIZE
} lightmeterToolsModes;

typedef enum {
    MAIN_VIEW,
    CONFIG_VIEW,

    /* Know menu Size*/
    VIEWS_SIZE
} lightmeterToolsViews;

typedef struct {
    lightmeterToolsModes current_mode;
    lightmeterToolsViews current_view;
} lightmeterMainView;

void draw_main_view(Canvas* canvas, lightmeterMainView* main_view, lightmeterI2CSender* sender);

lightmeterMainView* lightmeter_main_view_alloc();
void lightmeter_main_view_free(lightmeterMainView* main_view);
