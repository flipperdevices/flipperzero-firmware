/* util.h */

#ifndef UTIL_H_
#define UTIL_H_

int sdcard_open(MQX_FILE_PTR *com_handle, MQX_FILE_PTR *sdcard_handle, 
                MQX_FILE_PTR *partman_handle, MQX_FILE_PTR *filesystem_handle,
                char *partman_name, char *filesystem_name);

int sdcard_close(MQX_FILE_PTR *sdcard_handle, MQX_FILE_PTR *partman_handle,
                 MQX_FILE_PTR *filesystem_handle,
                 char *partman_name, char *filesystem_name);

#endif /* UTIL_H_ */
