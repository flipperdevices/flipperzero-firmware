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

/** Read data from a file 
 *
 * @param   buff        void* buffer to save data to
 * @param   to_read     size_t to_read size of data to read into buffer
 *
 * @return  size_t bytes read
 */
uint8_t mine_sweeper_storage_file_read(char* buff, size_t to_read);

#ifdef __cplusplus
}
#endif

#endif
