#include "mrtd_helpers.h"

uint8_t mrtd_bac_check_digit(const uint8_t* input, const size_t length) {
    const size_t num_weights = 3;
    uint8_t weights[] = {7, 3, 1};
    uint8_t check_digit = 0;
    uint8_t idx;

    for(size_t i=0; i<length; ++i) {
        uint8_t c = input[i];
        if(c >= 'A' && c <= 'Z') {
            idx = c - 'A' + 10;
        } else if(c >= 'a' && c <= 'z') {
            idx = c - 'a' + 10;
        } else if(c >= '0' && c <= '9') {
            idx = c - '0';
        } else {
            idx = 0;
        }
        check_digit = (check_digit + idx * weights[i%num_weights]) % 10;
    }
    return check_digit;
}

void mrtd_print_date(uint8_t* output, MrtdDate* date) {
    output[0] = (date->year / 10) + '0';
    output[1] = (date->year % 10) + '0';
    output[2] = (date->month / 10) + '0';
    output[3] = (date->month % 10) + '0';
    output[4] = (date->day / 10) + '0';
    output[5] = (date->day % 10) + '0';
}

// Safe size: MRTD_DOCNR_MAX_LENGTH + 1 (CD) + 6 (DOB) + 1 (CD) + 6 (DOE) + 1 (CD) + 1 (0x00)
// Test with:
// - DOCNR of size 9
// - DOCNR of size <9
// - DOCNR of size >9
// - DOCNR of size MRTD_DOCNR_MAX_LENGTH
bool mrtd_bac_get_kmrz(MrtdAuthData* auth, uint8_t* output, size_t output_size) {
    size_t idx = 0;
    size_t docnr_length = strlen(auth->doc_number);
    size_t cd_idx = 0;
    if(output_size < docnr_length + 16) {
        return false;
    }

    cd_idx = idx;
    memcpy(output+idx, auth->doc_number, docnr_length);
    idx += docnr_length;
    if(docnr_length < 9) {
        memset(output+idx, '<', 9-docnr_length);
        idx += 9-docnr_length;
    }

    output[idx++] = mrtd_bac_check_digit(output+cd_idx, docnr_length) + '0';

    cd_idx = idx;
    mrtd_print_date(output+idx, &auth->birth_date);
    idx += 6;
    output[idx++] = mrtd_bac_check_digit(output+cd_idx, 6) + '0';

    cd_idx = idx;
    mrtd_print_date(output+idx, &auth->expiry_date);
    idx += 6;
    output[idx++] = mrtd_bac_check_digit(output+cd_idx, 6) + '0';

    output[idx++] = '\x00';
    return true;
}
