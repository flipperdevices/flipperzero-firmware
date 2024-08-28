#pragma once

typedef enum {
    InfraredErrorCodeNone,
    InfraredErrorCodeFileOperationFailed,

    //Common signal errors
    InfraredErrorCodeSignalTypeUnkown,
    InfraredErrorCodeSignalNameNotFound,
    InfraredErrorCodeSignalUnableToReadType,
    InfraredErrorCodeSignalUnableToWriteType,

    //Raw signal errors
    InfraredErrorCodeSignalRawUnableToReadFrequency,
    InfraredErrorCodeSignalRawUnableToReadDutyCycle,
    InfraredErrorCodeSignalRawUnableToReadTimingsSize,
    InfraredErrorCodeSignalRawUnableToReadTooLongData,
    InfraredErrorCodeSignalRawUnableToReadData,

    InfraredErrorCodeSignalRawUnableToWriteFrequency,
    InfraredErrorCodeSignalRawUnableToWriteDutyCycle,
    InfraredErrorCodeSignalRawUnableToWriteData,

    //Message signal errors
    InfraredErrorCodeSignalMessageUnableToReadProtocol,
    InfraredErrorCodeSignalMessageUnableToReadAddress,
    InfraredErrorCodeSignalMessageUnableToReadCommand,
    InfraredErrorCodeSignalMessageIsInvalid,

    InfraredErrorCodeSignalMessageUnableToWriteProtocol,
    InfraredErrorCodeSignalMessageUnableToWriteAddress,
    InfraredErrorCodeSignalMessageUnableToWriteCommand,
} InfraredErrorCode;
