#pragma once

#include <gui/view.h>
#include "../helpers/tpms_types.h"
#include "../helpers/tpms_event.h"
#include <lib/flipper_format/flipper_format.h>

typedef struct TPMSReceiverInfo TPMSReceiverInfo;

void tpms_view_receiver_info_update(TPMSReceiverInfo* tpms_receiver_info, FlipperFormat* fff);

TPMSReceiverInfo* tpms_view_receiver_info_alloc();

void tpms_view_receiver_info_free(TPMSReceiverInfo* tpms_receiver_info);

View* tpms_view_receiver_info_get_view(TPMSReceiverInfo* tpms_receiver_info);
