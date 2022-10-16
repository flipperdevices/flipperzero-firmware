#pragma once

#include <gui/view.h>

typedef struct RebootView RebootView;

RebootView* reboot_view_alloc();

void reboot_view_free(RebootView* reboot_view);

View* reboot_view_get_view(RebootView* reboot_view);