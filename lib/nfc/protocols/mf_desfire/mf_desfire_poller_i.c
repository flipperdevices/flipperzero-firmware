#include "mf_desfire_poller_i.h"

#include <furi.h>

#define TAG "MfDesfirePoller"

MfDesfireError mf_desfire_process_error(NfcaError error) {
    MfDesfireError ret = MfDesfireErrorNone;

    switch(error) {
    case NfcaErrorNone:
        ret = MfDesfireErrorNone;
        break;
    case NfcaErrorNotPresent:
        ret = MfDesfireErrorNotPresent;
        break;
    case NfcaErrorColResFailed:
    case NfcaErrorCommunication:
    case NfcaErrorWrongCrc:
        ret = MfDesfireErrorProtocol;
        break;
    case NfcaErrorTimeout:
        ret = MfDesfireErrorTimeout;
        break;
    default:
        ret = MfDesfireErrorProtocol;
        break;
    }

    return ret;
}

MfDesfireError
    mf_desfire_poller_async_read_version(MfDesfirePoller* instance, MfDesfireVersion* data) {
    UNUSED(instance);
    UNUSED(data);

    MfDesfireError ret = MfDesfireErrorNone;
    return ret;
}
