#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef struct {
    uint8_t year;
    uint8_t month;
    uint8_t day;
} MrtdDate;

// NULL terminated document ID
#define MRTD_DOCNR_MAX_LENGTH 21

typedef enum {
    MrtdAuthMethodBac,
    MrtdAuthMethodPace,
} MrtdAuthMethod;

typedef struct {
    MrtdAuthMethod method;

    // BAC input fields
    MrtdDate birth_date;
    MrtdDate expiry_date;
    char doc_number[MRTD_DOCNR_MAX_LENGTH];

    //TODO: PACE
} MrtdAuthData;

uint8_t mrtd_bac_check_digit(const uint8_t* input, const uint8_t length);

void mrtd_print_date(uint8_t* output, MrtdDate* date);

bool mrtd_bac_get_kmrz(MrtdAuthData* auth, uint8_t* output, uint8_t output_size);

bool mrtd_bac_keys(const uint8_t* kseed, uint8_t* ksenc, uint8_t* ksmac);
