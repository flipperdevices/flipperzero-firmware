#include <furi.h>
#include <cli/cli.h>
#include "menu/menu.h"
#include "menu/menu_item.h"
#include "applications.h"
#include <assets_icons.h>
#include <api-hal.h>

/**
 * Start application
 */
bool app_loader_start(const char* name, const char* args);
