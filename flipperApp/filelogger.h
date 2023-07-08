/**
 * @file log.h
 * Furi Logging system
 */
#pragma once

#include <core/log.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Initialize logging */
void file_log_init();
void file_log_deinit();

/** Print log record
 * 
 * @param level 
 * @param tag 
 * @param format 
 * @param ... 
 */
void file_log_print_format(FuriLogLevel level, const char* tag, const char* format, ...)
    _ATTRIBUTE((__format__(__printf__, 3, 4)));

// /** Set log level
//  *
//  * @param[in]  level  The level
//  */
// void file_log_set_level(FuriLogLevel level);

// /** Get log level
//  *
//  * @return     The furi log level.
//  */
// FuriLogLevel file_log_get_level();

// /** Set log output callback
//  *
//  * @param[in]  puts  The puts callback
//  */
// void file_log_set_puts(FuriLogPuts puts);

/** Set timestamp callback
 *
 * @param[in]  timestamp  The timestamp callback
 */
void file_log_set_timestamp(FuriLogTimestamp timestamp);

/** Log methods
 *
 * @param      tag     The application tag
 * @param      format  The format
 * @param      ...     VA Args
 */
#define FILE_LOG_E(tag, format, ...) \
    file_log_print_format(FuriLogLevelError, tag, format, ##__VA_ARGS__)
#define FILE_LOG_W(tag, format, ...) \
    file_log_print_format(FuriLogLevelWarn, tag, format, ##__VA_ARGS__)
#define FILE_LOG_I(tag, format, ...) \
    file_log_print_format(FuriLogLevelInfo, tag, format, ##__VA_ARGS__)
#define FILE_LOG_D(tag, format, ...) \
    file_log_print_format(FuriLogLevelDebug, tag, format, ##__VA_ARGS__)
#define FILE_LOG_T(tag, format, ...) \
    file_log_print_format(FuriLogLevelTrace, tag, format, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif
