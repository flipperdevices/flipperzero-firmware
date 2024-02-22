/**
 * @file uf2.h
 * @brief UF2 file support functions.
 *
 * This is a minimal UF2 file implementation.
 *
 * UNsupported features:
 * - Non-flash blocks
 * - File containers
 * - Extended tags
 * - Md5 checksum
 *
 * Suported features:
 * - Family id (respective flag must be set)
 *
 * See https://github.com/Microsoft/uf2 for more information.
 */
#pragma once

#include <storage/storage.h>

/**
 * @brief Get the block count in a UF2 file.
 *
 * The file MUST be already open.
 *
 * Will fail if the file size is not evenly divisible
 * by 512 bytes (UF2 block size).
 *
 * @param[in] file pointer to the storage file instance.
 * @param[out] block_count pointer to the value to contain the block count.
 * @returns true on success, false otherwise.
 */
bool uf2_get_block_count(File* file, uint32_t* block_count);

/**
 * @brief Verify a single UF2 block.
 *
 * The file MUST be already open.
 *
 * Will fail if:
 * - the family id flag is set, but does not match the provided value,
 * - payload size does not match the provided value.
 *
 * @param[in] file pointer to the storage file instance.
 * @param[in] family_id family identifier to check against the respective header field.
 * @param[in] payload_size payload size to check agains the respective header field, in bytes.
 * @returns true on success, false otherwise.
 */
bool uf2_verify_block(File* file, uint32_t family_id, size_t payload_size);

/**
 * @brief Read the payload from a single UF2 block.
 *
 * @param[in] file pointer to the storage file instance.
 * @param[out] payload pointer to the buffer to contain the payload data.
 * @param[in] payload_size size of the payload buffer, in bytes.
 * @returns true on success, false otherwise.
 */
bool uf2_read_block(File* file, void* payload, size_t payload_size);
