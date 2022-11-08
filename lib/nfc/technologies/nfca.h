#pragma once

typedef struct {

} NfcaDevice;

bool nfca_poller_check_presence();

bool nfca_poller_activate();

void nfca_poller_sleep();
