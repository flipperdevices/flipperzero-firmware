#pragma once

#include <gui/view.h>

typedef struct GenieApp GenieApp;
typedef struct GenieSend GenieSend;

GenieSend* genie_send_alloc(GenieApp* app);
void genie_send_free(GenieSend* genie_send);
View* genie_send_get_view(GenieSend* genie_send);