#include <stdio.h>
#include <mbedtls/sha1.h>

#include "lib/nfc/protocols/mrtd_helpers.h"

// gcc -o test_mrtd_helpers -Ilib/mbedtls/include lib/nfc/protocols/mrtd_helpers.c lib/mbedtls/library/sha1.c lib/mbedtls/library/platform_util.c test_mrtd_helpers.c

#define COLOR_RED "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RESET "\033[0;0m"

void print_hex(const uint8_t* data, size_t length) {
    for(uint8_t i=0; i<length; ++i) {
        printf("%02X", data[i]);
    }
}

void test_mrtd_bac_check_digit(const uint8_t* input, const uint8_t exp_output) {
    uint8_t output = mrtd_bac_check_digit(input, strlen(input));
    if(output != exp_output) {
        printf(COLOR_RED "FAILED  - mrtd_bac_check_digit for %s is not %d, but %d\n" COLOR_RESET,
                input, exp_output, output);
        return;
    }

    printf(COLOR_GREEN "SUCCESS - mrtd_bac_check_digit for %s is %d\n" COLOR_RESET,
            input, output);
}

void test_bac_get_kmrz(MrtdAuthData* auth, const uint8_t* exp_output) {
    bool result;
    uint8_t buffer[255];

    result = mrtd_bac_get_kmrz(auth, buffer, 255);
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

void test_sha1(const uint8_t* data, const uint8_t* exp_output) {
    uint8_t hash[20];
    mbedtls_sha1(data, strlen(data), hash);

    if(memcmp(hash, exp_output, 20)) {
        printf(COLOR_RED "FAILED  - sha1 of %s, expected:\n", data);
        print_hex(exp_output, 20);
        printf(", result:\n");
    } else {
        printf(COLOR_GREEN "SUCCESS - sha1 of %s is: ", data);
    }

    print_hex(hash, 20);
    printf("\n" COLOR_RESET);
}

void test_mrtd_bac_keys(const uint8_t kseed[16], const uint8_t exp_ksenc[16], const uint8_t exp_ksmac[16]) {
    uint8_t ksenc[16];
    uint8_t ksmac[16];
    if(!mrtd_bac_keys(kseed, ksenc, ksmac)) {
        printf(COLOR_RED "FAILED  - mrtd_bac_keys returned FALSE for ");
        print_hex(kseed, 16);
        printf(COLOR_RESET "\n");
        return;
    }

    if(memcmp(exp_ksenc, ksenc, 16)) {
        printf(COLOR_RED "FAILED  - mrtd_bac_keys of ");
        print_hex(kseed, 16);
        printf(", expected ksenc:\n");
        print_hex(exp_ksenc, 16);
        printf(" is:\n");
        print_hex(ksenc, 16);
        return;
    } else if(memcmp(exp_ksmac, ksmac, 16)) {
        printf(COLOR_RED "FAILED  - mrtd_bac_keys of ");
        print_hex(kseed, 16);
        printf(", expected ksmac:\n");
        print_hex(exp_ksmac, 16);
        printf(" is:\n");
        print_hex(ksmac, 16);
        return;
    } else {
        printf(COLOR_GREEN "SUCCESS - mrtd_bac_keys of ");
        print_hex(kseed, 16);
        printf(" ksenc: ");
        print_hex(ksenc, 16);
        printf(" ksmac: ");
        print_hex(ksmac, 16);
        printf(COLOR_RESET "\n");
    }
}

int main(int argc, char** argv) {
    test_mrtd_bac_check_digit("D23145890734", 9);
    test_mrtd_bac_check_digit("340712", 7);
    test_mrtd_bac_check_digit("950712", 2);

    test_bac_get_kmrz(&(MrtdAuthData){
        .doc_number = "D23145890734",
        .birth_date = {34, 7, 12},
        .expiry_date = {95, 7, 12},
        }, "D23145890734934071279507122");
    test_bac_get_kmrz(&(MrtdAuthData){
        .doc_number = "L898902C",
        .birth_date = {69, 8, 6},
        .expiry_date = {94, 6, 23},
    }, "L898902C<369080619406236");

    test_sha1("L898902C<369080619406236", "\x23\x9a\xb9\xcb\x28\x2d\xaf\x66\x23\x1d\xc5\xa4\xdf\x6b\xfb\xae\xdf\x47\x75\x65");

    test_mrtd_bac_keys(
        "\x23\x9a\xb9\xcb\x28\x2d\xaf\x66\x23\x1d\xc5\xa4\xdf\x6b\xfb\xae",
        "\xab\x94\xfd\xec\xf2\x67\x4f\xdf\xb9\xb3\x91\xf8\x5d\x7f\x76\xf2",
        "\x79\x62\xd9\xec\xe0\x3d\x1a\xcd\x4c\x76\x08\x9d\xce\x13\x15\x43");

    return 0;
}
