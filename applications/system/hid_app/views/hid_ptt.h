#pragma once

#include <gui/view.h>

typedef struct Hid Hid;
typedef struct HidPtt HidPtt;

HidPtt* hid_ptt_alloc(Hid* bt_hid);

void hid_ptt_free(HidPtt* hid_ptt);

View* hid_ptt_get_view(HidPtt* hid_ptt);

void hid_ptt_set_connected_status(HidPtt* hid_ptt, bool connected);
