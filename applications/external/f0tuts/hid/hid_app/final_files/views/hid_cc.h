#pragma once

#include <gui/view.h>

typedef struct Hid Hid;
typedef struct HidCC HidCC;

HidCC* hid_cc_alloc(Hid* bt_hid);

void hid_cc_free(HidCC* hid_cc);

View* hid_cc_get_view(HidCC* hid_cc);

void hid_cc_set_connected_status(HidCC* hid_cc, bool connected);
