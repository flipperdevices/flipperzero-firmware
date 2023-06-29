#pragma once

#include "../nfc_custom_event.h"

#include <nfc/protocols/nfc_generic_event.h>

NfcCustomEvent nfc_poller_handler_read(NfcGenericEvent event, void* context);
