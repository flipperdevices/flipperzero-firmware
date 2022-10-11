#include <stdio.h>

#include "lib/nfc/protocols/mrtd_helpers.h"

#define COLOR_RED "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RESET "\033[0;0m"

void test_mrtd_bac_check_digit(const uint8_t* input, uint8_t exp_output) {
    uint8_t output = mrtd_bac_check_digit(input, strlen(input));
    if(output != exp_output) {
        printf(COLOR_RED "FAILED  - mrtd_bac_check_digit for %s is not %d, but %d\n" COLOR_RESET,
                input, exp_output, output);
        return;
    }

    printf(COLOR_GREEN "SUCCESS - mrtd_bac_check_digit for %s is %d\n" COLOR_RESET,
            input, output);
}

void test_bac_get_kmrz(MrtdAuthData* auth, uint8_t* exp_output) {
    bool result;
    uint8_t buffer[1000];

    result = mrtd_bac_get_kmrz(auth, buffer, 1000);
    if(!result) {
        printf(COLOR_RED "FAILED  - mrtd_bac_get_kmrz returned FALSE for" COLOR_RESET);
        return;
    }

    if(strcmp(exp_output, buffer)) {
        printf(COLOR_RED "FAILED  - mrtd_bac_get_kmrz expected:\n%s, result:\n%s\n" COLOR_RESET,
            exp_output,
            buffer);
    }

    printf(COLOR_GREEN "SUCCESS - mrtd_bac_get_kmrz is: %s\n" COLOR_RESET,
        buffer);
}

int main(int argc, char** argv) {
    test_mrtd_bac_check_digit("D23145890734", 9);
    test_mrtd_bac_check_digit("340712", 7);
    test_mrtd_bac_check_digit("950712", 2);

    MrtdAuthData mad1 = {
        .doc_number = "D23145890734",
        .birth_date = {34, 7, 12},
        .expiry_date = {95, 7, 12},
    };
    test_bac_get_kmrz(&mad1, "D23145890734934071279507122");
    test_bac_get_kmrz(&(MrtdAuthData){
        .doc_number = "L898902C",
        .birth_date = {69, 8, 6},
        .expiry_date = {94, 6, 23},
    }, "L898902C<369080619406236");

    return 0;
}
