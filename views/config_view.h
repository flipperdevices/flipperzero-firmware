#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include "lightmeter_icons.h"

typedef struct {
    uint16_t iso;
    uint16_t nd;
} lightmeterConfig;

typedef enum {
    CONFIG_ISO,
    CONFIG_ND,

    /* Know menu Size*/
    CONFIG_SIZE
} lightmeterConfigItems;

typedef struct {
    lightmeterConfigItems current_item;
} lightmeterConfigView;

void draw_config_view(Canvas* canvas, lightmeterConfigView* config_view, lightmeterConfig* config);
