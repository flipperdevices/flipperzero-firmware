#ifndef STORAGE_HELPER_H
#define STORAGE_HELPER_H

#include <storage/storage.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Write data to file 
 *
 * @param   format      const char* printf style format string 
 * @param   ...         variable argument list for format string 
 *
 * @return  boolean value true/false
 */
bool mine_sweeper_storage_file_write(const char* format, ...)
    _ATTRIBUTE((__format__(__printf__, 1, 2)));

#ifdef __cplusplus
}
#endif

#endif
