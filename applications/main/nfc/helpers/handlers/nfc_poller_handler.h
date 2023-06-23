#pragma once

#include <nfc/protocols/nfc_poller_base.h>

#include "../nfc_custom_event.h"

NfcCustomEvent nfc_poller_handler_read(NfcPollerEvent event, void* context);
