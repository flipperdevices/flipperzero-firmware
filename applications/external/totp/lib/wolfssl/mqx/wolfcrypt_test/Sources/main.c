/* main.c */

#include "main.h"

/* SD card open/close utility functions */
#include "util.h"

#if !BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined \
    non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. \
    Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in \
    user_config.h and recompile BSP with this option.
#endif

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    /*  Task number, Entry point, Stack, Pri, String, Auto? */
    {MAIN_TASK,   Main_task,   20000,  9,   "main", MQX_AUTO_START_TASK},
    {0,           0,           0,     0,   0,      0,                 }
};

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

/* func_args from test.h */
typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;

/*TASK*-----------------------------------------------------------------
 * Function Name  : Main_task
 * Comments       :
 *    This task opens the SD card device and runs the 
 *    wolfCrypt test functions located in test.c.
 *END------------------------------------------------------------------*/

void Main_task(uint32_t initial_data)
{
    int          ret = 0;
    func_args    args;
    char         filesystem_name[] = "a:";
    char         partman_name[]    = "pm:";
    MQX_FILE_PTR com_handle, sdcard_handle, filesystem_handle, partman_handle;
    
    ret = sdcard_open(&com_handle, &sdcard_handle, &partman_handle,
                      &filesystem_handle, partman_name, filesystem_name);
    if (ret != 0) {
        printf("error: sdcard_open(), ret = %d\n", ret);
        _mqx_exit(1);
    }
    printf("SD card installed to %s\n", filesystem_name);
    
    wolfcrypt_test(&args);
    
    ret = sdcard_close(&sdcard_handle, &partman_handle,
                       &filesystem_handle, partman_name, filesystem_name);
    if (ret != 0) {
        printf("error: sdcard_close(), ret = %d\n", ret);
        _mqx_exit(1);
    }
    printf("SD card uninstalled.\n");
    
    _mqx_exit(0);

}

