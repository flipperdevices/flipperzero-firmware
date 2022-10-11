#include <stdio.h>
#include <mbedtls/sha1.h>
#include <mbedtls/des.h>

#include "lib/nfc/protocols/mrtd_helpers.h"

// gcc -o test_mrtd_helpers -Wall -Ilib/mbedtls/include lib/nfc/protocols/mrtd_helpers.c lib/mbedtls/library/sha1.c lib/mbedtls/library/des.c lib/mbedtls/library/platform_util.c test_mrtd_helpers.c

#define COLOR_RED "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RESET "\033[0;0m"

void print_hex(const uint8_t* data, size_t length) {
    for(uint8_t i=0; i<length; ++i) {
        printf("%02X", data[i]);
    }
}

void test_mrtd_bac_check_digit(const char* input, const uint8_t exp_output) {
    uint8_t output = mrtd_bac_check_digit(input, strlen(input));
    if(output != exp_output) {
        printf(COLOR_RED "FAILED  - mrtd_bac_check_digit for %s is not %d, but %d\n" COLOR_RESET,
                input, exp_output, output);
        return;
    }

    printf(COLOR_GREEN "SUCCESS - mrtd_bac_check_digit for %s is %d\n" COLOR_RESET,
            input, output);
}

void test_bac_get_kmrz(MrtdAuthData* auth, const char* exp_output) {
    bool result;
    char buffer[255];

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
    mbedtls_sha1(data, strlen((char*)data), hash);

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

void test_mrtd_bac_keys_from_seed(const uint8_t kseed[16], const uint8_t exp_ksenc[16], const uint8_t exp_ksmac[16]) {
    uint8_t ksenc[16];
    uint8_t ksmac[16];
    if(!mrtd_bac_keys_from_seed(kseed, ksenc, ksmac)) {
        printf(COLOR_RED "FAILED  - mrtd_bac_keys_from_seed returned FALSE for ");
        print_hex(kseed, 16);
        printf(COLOR_RESET "\n");
        return;
    }

    if(memcmp(exp_ksenc, ksenc, 16)) {
        printf(COLOR_RED "FAILED  - mrtd_bac_keys_from_seed of ");
        print_hex(kseed, 16);
        printf(", expected ksenc:\n");
        print_hex(exp_ksenc, 16);
        printf(" is:\n");
        print_hex(ksenc, 16);
        return;
    } else if(memcmp(exp_ksmac, ksmac, 16)) {
        printf(COLOR_RED "FAILED  - mrtd_bac_keys_from_seed of ");
        print_hex(kseed, 16);
        printf(", expected ksmac:\n");
        print_hex(exp_ksmac, 16);
        printf(" is:\n");
        print_hex(ksmac, 16);
        return;
    } else {
        printf(COLOR_GREEN "SUCCESS - mrtd_bac_keys_from_seed of ");
        print_hex(kseed, 16);
        printf(" ksenc: ");
        print_hex(ksenc, 16);
        printf(" ksmac: ");
        print_hex(ksmac, 16);
        printf(COLOR_RESET "\n");
    }
}

void test_mrtd_bac_keys(MrtdAuthData* auth, const uint8_t exp_ksenc[16], const uint8_t exp_ksmac[16]) {
    uint8_t ksenc[16];
    uint8_t ksmac[16];
    if(!mrtd_bac_keys(auth, ksenc, ksmac)) {
        printf(COLOR_RED "FAILED  - mrtd_bac_keys returned FALSE\n" COLOR_RESET);
        return;
    }

    if(memcmp(exp_ksenc, ksenc, 16)) {
        printf(COLOR_RED "FAILED  - mrtd_bac_keys, expected ksenc:\n");
        print_hex(exp_ksenc, 16);
        printf(" is:\n");
        print_hex(ksenc, 16);
        return;
    } else if(memcmp(exp_ksmac, ksmac, 16)) {
        printf(COLOR_RED "FAILED  - mrtd_bac_keys, expected ksmac:\n");
        print_hex(exp_ksmac, 16);
        printf(" is:\n");
        print_hex(ksmac, 16);
        return;
    } else {
        printf(COLOR_GREEN "SUCCESS - mrtd_bac_keys ksenc: ");
        print_hex(ksenc, 16);
        printf(" ksmac: ");
        print_hex(ksmac, 16);
        printf(COLOR_RESET "\n");
    }
}

void test_mrtd_bac_encrypt(uint8_t* data, size_t data_length, uint8_t* key, uint8_t* exp_output, size_t exp_output_length) {
    uint8_t buffer[256];

    // buffer size must be at least ((data_length+8)/8)*8
    mrtd_bac_encrypt(data, data_length, key, buffer);

    if(memcmp(exp_output, buffer, exp_output_length)) {
        printf(COLOR_RED "FAILED  - mrtd_bac_encrypt, expected output:\n");
        print_hex(exp_output, exp_output_length);
        printf(" is:\n");
        print_hex(buffer, exp_output_length);
        return;
    } else {
        printf(COLOR_GREEN "SUCCESS - mrtd_bac_encrypt output: ");
        print_hex(buffer, exp_output_length);
        printf(COLOR_RESET "\n");
    }
}

void test_mrtd_bac_padded_mac(uint8_t* data, size_t data_length, uint8_t* key, uint8_t* exp_output, size_t exp_output_length) {
    uint8_t mac[8];
    if(!mrtd_bac_padded_mac(data, data_length, key, mac)) {
        printf("ERROR BAC MAC");
        return;
    }

    if(memcmp(exp_output, mac, exp_output_length)) {
        printf(COLOR_RED "FAILED  - mrtd_bac_padded_mac, expected output:\n");
        print_hex(exp_output, exp_output_length);
        printf(" is:\n");
        print_hex(mac, 8);
        return;
    } else {
        printf(COLOR_GREEN "SUCCESS - mrtd_bac_padded_mac output: ");
        print_hex(mac, 8);
        printf(COLOR_RESET "\n");
    }
}

void test_mrtd_bac_decrypt_verify(const uint8_t* data, size_t data_length, uint8_t* key_enc, uint8_t* key_mac, uint8_t* exp_output, size_t exp_output_length, bool should_verify) {
    uint8_t buffer[256];

    bool result = mrtd_bac_decrypt_verify(data, data_length, key_enc, key_mac, buffer);
    if(result != should_verify) {
        printf(COLOR_RED "FAILED  - mrtd_bac_decrypt_verify, expected verify: %d, but is: %d\n", should_verify, result);
        return;
    }

    if(memcmp(exp_output, buffer, exp_output_length)) {
        printf(COLOR_RED "FAILED  - mrtd_bac_decrypt_verify, expected output:\n");
        print_hex(exp_output, exp_output_length);
        printf(" is:\n");
        print_hex(buffer, 32);
        return;
    }

    printf(COLOR_GREEN "SUCCESS - mrtd_bac_decrypt_verify output: ");
    print_hex(buffer, exp_output_length);
    printf(COLOR_RESET "\n");
}

void test_mrtd_ssc_from_data(const uint8_t* rnd_ic, const uint8_t* rnd_ifd, uint64_t exp_ssc) {
    uint64_t ssc_long = mrtd_ssc_from_data(rnd_ic, rnd_ifd);

    if(ssc_long != exp_ssc) {
        printf(COLOR_RED "FAILED  - mrtd_ssc_from_data, expected ssc: %016lx, but is: %016lx\n" COLOR_RESET, exp_ssc, ssc_long);
        return;
    }

    printf(COLOR_GREEN "SUCCESS - mrtd_ssc_from_data output: %016lx\n" COLOR_RESET, ssc_long);
}

int main(int argc, char** argv) {
    test_mrtd_bac_check_digit("D23145890734", 9);
    test_mrtd_bac_check_digit("340712", 7);
    test_mrtd_bac_check_digit("950712", 2);

    test_bac_get_kmrz(&(MrtdAuthData){
        .doc_number = "D23145890734",
        .birth_date = {34, 7, 12},
        .expiry_date = {95, 7, 12},
        }, "D23145890734934071279507122"
    );
    test_bac_get_kmrz(&(MrtdAuthData){
            .doc_number = "L898902C",
            .birth_date = {69, 8, 6},
            .expiry_date = {94, 6, 23},
        }, "L898902C<369080619406236"
    );

    test_sha1((uint8_t*)"L898902C<369080619406236", (uint8_t*)"\x23\x9a\xb9\xcb\x28\x2d\xaf\x66\x23\x1d\xc5\xa4\xdf\x6b\xfb\xae\xdf\x47\x75\x65");

    test_mrtd_bac_keys_from_seed(
        (uint8_t*)"\x23\x9a\xb9\xcb\x28\x2d\xaf\x66\x23\x1d\xc5\xa4\xdf\x6b\xfb\xae",
        (uint8_t*)"\xab\x94\xfd\xec\xf2\x67\x4f\xdf\xb9\xb3\x91\xf8\x5d\x7f\x76\xf2",
        (uint8_t*)"\x79\x62\xd9\xec\xe0\x3d\x1a\xcd\x4c\x76\x08\x9d\xce\x13\x15\x43"
    );

    test_mrtd_bac_keys(&(MrtdAuthData){
            .doc_number = "L898902C",
            .birth_date = {69, 8, 6},
            .expiry_date = {94, 6, 23},
        },
        (uint8_t*)"\xab\x94\xfd\xec\xf2\x67\x4f\xdf\xb9\xb3\x91\xf8\x5d\x7f\x76\xf2",
        (uint8_t*)"\x79\x62\xd9\xec\xe0\x3d\x1a\xcd\x4c\x76\x08\x9d\xce\x13\x15\x43"
    );

    test_mrtd_bac_encrypt(
        /*input*/ (uint8_t*)"\x78\x17\x23\x86\x0C\x06\xC2\x26\x46\x08\xF9\x19\x88\x70\x22\x12\x0B\x79\x52\x40\xCB\x70\x49\xB0\x1C\x19\xB3\x3E\x32\x80\x4F\x0B",
        /*size*/  32,
        /*key*/  (uint8_t*)"\xAB\x94\xFD\xEC\xF2\x67\x4F\xDF\xB9\xB3\x91\xF8\x5D\x7F\x76\xF2",
        /*exp output*/ (uint8_t*)"\x72\xC2\x9C\x23\x71\xCC\x9B\xDB\x65\xB7\x79\xB8\xE8\xD3\x7B\x29\xEC\xC1\x54\xAA\x56\xA8\x79\x9F\xAE\x2F\x49\x8F\x76\xED\x92\xF2",
        /*exp_output_size*/ 32
    );

    test_mrtd_bac_padded_mac(
        /*input*/ (uint8_t*)"\x72\xC2\x9C\x23\x71\xCC\x9B\xDB\x65\xB7\x79\xB8\xE8\xD3\x7B\x29\xEC\xC1\x54\xAA\x56\xA8\x79\x9F\xAE\x2F\x49\x8F\x76\xED\x92\xF2",
        /*size*/  32,
        /*key*/  (uint8_t*)"\x79\x62\xD9\xEC\xE0\x3D\x1A\xCD\x4C\x76\x08\x9D\xCE\x13\x15\x43",
        /*exp output*/ (uint8_t*)"\x5F\x14\x48\xEE\xA8\xAD\x90\xA7",
        /*exp_output_size*/ 8
    );

    test_mrtd_bac_decrypt_verify(
        /*input*/ (uint8_t*)"\x46\xB9\x34\x2A\x41\x39\x6C\xD7\x38\x6B\xF5\x80\x31\x04\xD7\xCE\xDC\x12\x2B\x91\x32\x13\x9B\xAF\x2E\xED\xC9\x4E\xE1\x78\x53\x4F\x2F\x2D\x23\x5D\x07\x4D\x74\x49",
        /*size*/  40,
        /*key_enc*/  (uint8_t*)"\xAB\x94\xFD\xEC\xF2\x67\x4F\xDF\xB9\xB3\x91\xF8\x5D\x7F\x76\xF2",
        /*key_mac*/  (uint8_t*)"\x79\x62\xD9\xEC\xE0\x3D\x1A\xCD\x4C\x76\x08\x9D\xCE\x13\x15\x43",
        /*exp output*/ (uint8_t*)"\x46\x08\xF9\x19\x88\x70\x22\x12\x78\x17\x23\x86\x0C\x06\xC2\x26\x0B\x4F\x80\x32\x3E\xB3\x19\x1C\xB0\x49\x70\xCB\x40\x52\x79\x0B",
        /*exp_output_size*/ 32,
        /*should_verify*/ 1
    );

    //TODO: test that does not verify

    test_mrtd_bac_keys_from_seed(
        (uint8_t*)"\x00\x36\xD2\x72\xF5\xC3\x50\xAC\xAC\x50\xC3\xF5\x72\xD2\x36\x00",
        (uint8_t*)"\x97\x9E\xC1\x3B\x1C\xBF\xE9\xDC\xD0\x1A\xB0\xFE\xD3\x07\xEA\xE5",
        (uint8_t*)"\xF1\xCB\x1F\x1F\xB5\xAD\xF2\x08\x80\x6B\x89\xDC\x57\x9D\xC1\xF8"
    );

    uint8_t* rnd_ic = (uint8_t*)"\x46\x08\xF9\x19\x88\x70\x22\x12";
    uint8_t* rnd_ifd = (uint8_t*)"\x78\x17\x23\x86\x0C\x06\xC2\x26";

    test_mrtd_ssc_from_data(rnd_ic, rnd_ifd, 0x887022120C06C226);

    return 0;
}
