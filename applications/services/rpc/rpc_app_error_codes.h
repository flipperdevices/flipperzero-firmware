#pragma once

/**
 * @brief Enumeration of possible error codes which are commom among all applications which use rpc.
 * If application needs to introduce some custom error codes, then similar type enum must be defined
 * in scope of this application and values must begin from RpcAppSystemErrorCodesReserved. All handling
 * and interpreting of such custom error codes must be done within that application. Also be aware that
 * same code for different applications can have different meaning.
 */
typedef enum {
    RpcAppSystemErrorCodeNone, /** There are no errors */
    RpcAppSystemErrorCodeParseFile, /** File parsing error, or wrong file structure, or missing required parameters. more accurate data can be obtained through the debug port */

    RpcAppSystemErrorCodesReserved =
        100, /** Values below are reserved for application specific errors */
} RpcAppSystemErrorCodes;
