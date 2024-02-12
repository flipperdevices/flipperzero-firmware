#pragma once

#include <lib/nfc/protocols/iso14443_3a/iso14443_3a.h>
#include <furi.h>

/**
 * Structure to store data TuLlave collected data
 * from the card
*/
typedef struct {
    uint64_t balance;
    FuriString* card_number;
    FuriString* nfc_uid;
    bool data_collected;
} TuLlaveData;

/**
 * A method helper that converts a series of bytes into hexadecimal form
 * used to convert UID in text plain
 * 
 * @param str
 *          String where the hexadecimal value will be stored
 * 
 * @param data
 *          buffer with bytes data to be parsed
 * 
 * @param size
 *          buffer size
 *  
*/
void tullave_iso14443_4a_format_bytes(FuriString* str, const uint8_t* const data, size_t size);

/**
 * Converts a bytes array into their respective representation of
 * uint64 using big endian
 * 
 * @param src
 *          bytes array to be parsed
 * @param length
 *          length of bytes array.
 * 
*/
uint64_t uint8_to_integer_big_endian(const uint8_t* src, size_t length);

/**
 * Frees the buffer required to store TuLlave collected data 
 * 
 * @param tullave_info
 *          pointer to tullave data buffer
*/
void tullave_data_free(TuLlaveData* tullave_info);

/**
 * Allocates and initializes the required memory to store TuLlave collected data
*/
TuLlaveData* tullave_data_alloc();