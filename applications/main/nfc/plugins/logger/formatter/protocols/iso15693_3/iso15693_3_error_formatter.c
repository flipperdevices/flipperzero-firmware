#include "iso15693_3_error_formatter.h"

static const char* errors[] = {
    [Iso15693_3ErrorNone] = "None",
    [Iso15693_3ErrorNotPresent] = "Not Present",
    [Iso15693_3ErrorBufferEmpty] = "Buffer Empty",
    [Iso15693_3ErrorBufferOverflow] = "Buffer Overflow",
    [Iso15693_3ErrorFieldOff] = "Field Off",
    [Iso15693_3ErrorWrongCrc] = "Wrong Crc",
    [Iso15693_3ErrorTimeout] = "Timeout",
    [Iso15693_3ErrorFormat] = "Format Error",
    [Iso15693_3ErrorIgnore] = "Ignore",
    [Iso15693_3ErrorNotSupported] = "Not Supported",
    [Iso15693_3ErrorUidMismatch] = "Uid Mismatch",
    [Iso15693_3ErrorFullyHandled] = "Fully Handled",
    [Iso15693_3ErrorUnexpectedResponse] = "Unexpected Response",
    [Iso15693_3ErrorInternal] = "Internal Error",
    [Iso15693_3ErrorCustom] = "Custom",
    [Iso15693_3ErrorUnknown] = "Unknown Error",
};

const char* iso15693_3_format_error(Iso15693_3Error error) {
    return errors[error];
}
