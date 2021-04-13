#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct Version_;
typedef struct Version_ Version;

/**
 * Gets current running firmware version handle.
 * You can store it somewhere. But if you want to retrieve data,
 * you have to use 'version_*_get()' set of functions.
 * Also, 'version_*_get()' imply to use this
 * handle if no handle (NULL_PTR) provided.
 *
 * @return Handle to version data.
 */
const Version *version_get(void);

/**
 * Gets Bootloader version handle.
 * Have to be used with 'version_*_get()' to retrieve data.
 *
 * @return Handle to bootloader version data.
 */
const Version *version_boot_get(void);

/**
 * Gets git hash of build commit.
 * 
 * @param   v - ptr to version handle. If zero - gets current running fw info.
 * @return  git hash 
 */
const char *version_githash_get(const Version *v);

/**
 * Gets git branch of build commit.
 * 
 * @param   v - ptr to version handle. If zero - gets current running fw info.
 * @return  git branch
 */
const char *version_gitbranch_get(const Version *v);

/**
 * Gets git number of build commit.
 * 
 * @param   v - ptr to version handle. If zero - gets current running fw info.
 * @return  number of commit
 */
const char *version_gitbranchnum_get(const Version *v);

/**
 * Gets build date.
 * 
 * @param   v - ptr to version handle. If zero - gets current running fw info.
 * @return  build date
 */
const char *version_build_date_get(const Version *v);

#ifdef __cplusplus
}
#endif

