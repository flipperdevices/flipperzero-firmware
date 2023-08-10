/* util.c */

#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <mfs.h>
#include <sdcard.h>
#include <spi.h>
#include <part_mgr.h>

#include "util.h"

#if !BSPCFG_ENABLE_IO_SUBSYSTEM
    #error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined \
        non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
    #error This application requires BSP_DEFAULT_IO_CHANNEL to be not \
        NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero \
        in user_config.h and recompile BSP with this option.
#endif

#if defined BSP_SDCARD_ESDHC_CHANNEL
    #if ! BSPCFG_ENABLE_ESDHC
        #error This application requires BSPCFG_ENABLE_ESDHC defined \
            non-zero in user_config.h. Please recompile libraries with \
            this option.
    #endif
#elif defined BSP_SDCARD_SDHC_CHANNEL
    #if ! BSPCFG_ENABLE_SDHC
        #error This application requires BSPCFG_ENABLE_SDHC defined \
            non-zero in user_config.h. Please recompile libraries with \
            this option.
    #endif
#endif

#if defined (BSP_SDCARD_SPI_CHANNEL)
    #define SDCARD_COM_CHANNEL BSP_SDCARD_SPI_CHANNEL
#elif defined (BSP_SDCARD_ESDHC_CHANNEL)
    #define SDCARD_COM_CHANNEL BSP_SDCARD_ESDHC_CHANNEL
#elif defined (BSP_SDCARD_SDHC_CHANNEL)
    #define SDCARD_COM_CHANNEL BSP_SDCARD_SDHC_CHANNEL
#else
    #error "SDCARD low level communication device not defined!"
#endif

int sdcard_open(MQX_FILE_PTR *com_handle, MQX_FILE_PTR *sdcard_handle,
                MQX_FILE_PTR *partman_handle, MQX_FILE_PTR *filesystem_handle,
                char *partman_name, char *filesystem_name)
{
	_mqx_int	error_code;
	_mqx_uint	param;

	/* Open low level communication device */
	*com_handle = fopen(SDCARD_COM_CHANNEL, NULL);

	if (NULL == *com_handle) {
		printf("Error installing communication handle.\n");
		return -60;
	}

	/* Install SD card device */
	error_code = _io_sdcard_install("sdcard:", (void *)&_bsp_sdcard0_init,
			*com_handle);
	if (error_code != MQX_OK) {
		printf("Error installing SD card device (0x%x)\n", error_code);
		return -61;
	}

	_time_delay(200);

	/* Open the device which MFS will be installed on */
	*sdcard_handle = fopen("sdcard:", 0);
	if (*sdcard_handle == NULL) {
		printf("Unable to open SD card device.\n");
		return -62;
	}

	/* Install partition manager over SD card driver */
	error_code = _io_part_mgr_install(*sdcard_handle, partman_name, 0);
	if (error_code != MFS_NO_ERROR) {
		printf("Error installing partition manager: %s\n", MFS_Error_text(
				(uint32_t) error_code));
		return -63;
	}

	/* Open partition manager */
	*partman_handle = fopen(partman_name, NULL);
	if (*partman_handle == NULL) {
		error_code = ferror(*partman_handle);
		printf("Error opening partition manager: %s\n", MFS_Error_text(
				(uint32_t) error_code));
		return -64;
	}

	/* Validate partition 1 */
	param = 1;
	error_code = _io_ioctl(*partman_handle, IO_IOCTL_VAL_PART, &param);
	if (error_code == MQX_OK) {
		/* Install MFS over partition 1 */
		error_code = _io_mfs_install(*partman_handle, filesystem_name, param);
		if (error_code != MFS_NO_ERROR) {
			printf("Error initializing MFS over partition: %s\n",
					MFS_Error_text((uint32_t) error_code));
			return -65;
		}

	} else {
		/* Install MFS over SD card driver */
		error_code = _io_mfs_install(*sdcard_handle, filesystem_name,
                (_file_size) 0);
		if (error_code != MFS_NO_ERROR) {
			printf("Error initializing MFS: %s\n", MFS_Error_text(
					(uint32_t) error_code));
			return -66;
		}
	} /* end Validate partition 1 */

	/* Open file system */
	*filesystem_handle = fopen(filesystem_name, NULL);
	error_code = ferror(*filesystem_handle);
	if ((error_code != MFS_NO_ERROR) && (error_code != MFS_NOT_A_DOS_DISK)) {
		printf("Error opening filesystem: %s\n", MFS_Error_text(
				(uint32_t) error_code));
		return -67;
	}
	if (error_code == MFS_NOT_A_DOS_DISK) {
		printf("NOT A DOS DISK! You must format to continue.\n");
		return -68;
	}

	return 0;
}

int sdcard_close(MQX_FILE_PTR *sdcard_handle, MQX_FILE_PTR *partman_handle,
                 MQX_FILE_PTR *filesystem_handle,
                 char *partman_name, char *filesystem_name)
{
	_mqx_int	error_code;

	/* Close the filesystem */
	if (MQX_OK != fclose(*filesystem_handle)) {
		printf("Error closing filesystem.\n");
		return -69;
	}
	*filesystem_handle = NULL;

	/* Uninstall MFS  */
	error_code = _io_dev_uninstall(filesystem_name);
	if (error_code != MFS_NO_ERROR) {
		printf("Error uninstalling filesystem.\n");
		return -70;
	}

	/* Close partition manager */
	if (MQX_OK != fclose(*partman_handle)) {
		printf("Unable to close partition manager.\n");
		return -71;
	}
	*partman_handle = NULL;

	/* Uninstall partition manager  */
	error_code = _io_dev_uninstall(partman_name);
	if (error_code != MFS_NO_ERROR) {
		printf("Error uninstalling partition manager.\n");
		return -72;
	}

	/* Close the SD card device */
	if (MQX_OK != fclose(*sdcard_handle)) {
		printf("Unable to close SD card device.\n");
		return -73;
	}
	*sdcard_handle = NULL;

	return 0;
}

/* EOF */
