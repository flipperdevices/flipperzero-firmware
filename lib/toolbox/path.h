#pragma once

#include <m-string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Extract filename without extension from path.
 * 
 * @param path path string
 * @param filename output filename string. Must be initialized before.
 */
void path_extract_filename_no_ext(const char* path, string_t filename);

/**
 * @brief Extract last path component
 * 
 * @param path path string
 * @param filename output string. Must be initialized before.
 */
void path_extract_basename(const char* path, string_t basename);

/**
 * @brief Extract path, except for last component
 * 
 * @param path path string
 * @param filename output string. Must be initialized before.
 */
void path_extract_dirname(const char* path, string_t dirname);

void path_append(string_t path, const char* suffix);

void path_concat(const char* path, const char* suffix, string_t out_path);


#ifdef __cplusplus
}
#endif