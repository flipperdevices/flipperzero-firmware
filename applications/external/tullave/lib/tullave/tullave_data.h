#pragma once

#include <nfc/protocols/iso14443_4a/iso14443_4a.h>
#include <toolbox/simple_array.h>
#include <furi.h>

#define STR_TIME_BUFFER_MAX 20
#define TULLAVE_TIME_BYTES_LEN 7

/**
 * List the possible transaction types that can be read from the card.
*/
typedef enum {
    BusTravel = 0x01,
    CardReload = 0x02,
    CardReloadCancelled = 0x04,
    UnknownTransaction = 0xFF,
} TuLlaveTransactionType;

/**
 * Custom structure to store transaction time
 * it is pretty similar to time.h, but the former cannot
 * be implemented due Flipper API limitations
*/
typedef struct {
    uint8_t mday;
    uint8_t mon;
    uint16_t year;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} TuLlaveTime;

/**
 * Structure to store TuLlave transaction data
*/
typedef struct {
    TuLlaveTime* datetime;
    TuLlaveTransactionType type;
    uint64_t final_balance;
    uint64_t cost;
} TuLlaveTransaction;

/**
 * Structure to store data TuLlave collected data
 * from the card
*/
typedef struct {
    uint64_t balance;
    FuriString* card_number;
    SimpleArray* transaction_history;
    Iso14443_4aData* iso_data;
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
void tullave_data_format_bytes(FuriString* str, const uint8_t* const data, size_t size);

/**
 * A method helper that converts the TuLlaveTime struct in a string form
 * 
 * @param time 
 *          TuLlaveTime structure
 * 
 * @param time_str
 *          The string where the printed time will be printed
 * 
*/
void tullave_data_format_time(TuLlaveTime* time, FuriString* time_str);

/**
 * Sets the Unique Identifier for the current read in the dst_str, part of the ISO1443-3a 
 * data
 * 
 * @param data
 *          The instance of TuLlaveData where the UID is allocated
 * 
 * @param dst_str
 *          The FuriString destination where the UID will be set
 * 
*/
void tullave_data_get_uid_str(TuLlaveData* data, FuriString* dst_str);

/**
 * Copy the data that was orginally extraced by the NFC into the TuLlaveData 
 * structure, this is required to preserve the initial data collected by the NFC
 * 
 * @param data
 *          TuLlaveData destination structure
 * 
 * @param iso_data
 *          The data that was originally extracted from the NFC poller and device APIs.
 * 
*/
void tullave_data_set_iso_data(TuLlaveData* data, const Iso14443_4aData* iso_data);

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
uint64_t tullave_data_uint8_to_balance(const uint8_t* src, size_t length);

/**
 * Converts a byte array into their respective representation of
 * TuLlaveTime the byte array contains a hexadecimal (visual) representation
 * of the date
 * 
 * @param src
 *          bytes array to be parsed
 * 
 * @param length
 *          bytes array length. It should greater or equal to 7. In case a greater array is passed,
 *          the method will only read first 7 bytes.
 * 
 * @param time
 *          target structure of TuLlaveTime
 * 
*/
void tullave_data_uint8_to_time(const uint8_t* src, size_t length, TuLlaveTime* time);

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