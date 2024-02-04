#pragma once

#include <gui/modules/variable_item_list.h>
#include <gui/view.h>

typedef struct GenieConfig GenieConfig;
typedef struct GenieApp GenieApp;

GenieConfig* genie_config_alloc(GenieApp* app);
void genie_config_free(GenieConfig* genie_config);
View* genie_config_get_view(GenieConfig* genie_config);

bool select_genie_file(GenieConfig* config);