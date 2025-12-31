#include "slix_error_formatter.h"

static const char* errors[] = {
    [SlixErrorNone] = "None",
    [SlixErrorTimeout] = "Timeout",
    [SlixErrorFormat] = "Format",
    [SlixErrorNotSupported] = "Not Supported",
    [SlixErrorInternal] = "Internal Error",
    [SlixErrorWrongPassword] = "Wrong Password",
    [SlixErrorUidMismatch] = "Uid Mismatch",
    [SlixErrorUnknown] = "Unknown Error",
};

const char* slix_format_error(SlixError error) {
    return errors[error];
}
