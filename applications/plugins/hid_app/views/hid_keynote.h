#pragma once

#include <gui/view.h>

typedef struct HidKeynote HidKeynote;

HidKeynote* hid_keynote_alloc();

void hid_keynote_free(HidKeynote* hid_keynote);

View* hid_keynote_get_view(HidKeynote* hid_keynote);

void hid_keynote_set_connected_status(HidKeynote* hid_keynote, bool connected);

void hid_keynote_set_conn_type(HidKeynote* hid_keynote, bool is_bluetooth);