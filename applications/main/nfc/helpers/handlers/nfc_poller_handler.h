#pragma once

#include "../nfc_custom_event.h"

#include <nfc/protocols/nfc_poller_common.h>

NfcCustomEvent nfc_poller_handler_read(NfcPollerEvent event, void* context);
