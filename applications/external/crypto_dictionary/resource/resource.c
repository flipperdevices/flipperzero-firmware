#include "resource.h"

Topic symmetric_cipher[] = {
    {"1-Blowfish", NULL, APP_ASSETS_PATH("symmetric_cipher/blowfish.txt")},
    {"2-Camellia", NULL, APP_ASSETS_PATH("symmetric_cipher/camellia.txt")},
    {"3-CAST-128", NULL, APP_ASSETS_PATH("symmetric_cipher/cast128.txt")},
    {"4-CAST-256", NULL, APP_ASSETS_PATH("symmetric_cipher/cast256.txt")},
    {"5-DES", NULL, APP_ASSETS_PATH("symmetric_cipher/des.txt")},
    {"6-IDEA", NULL, APP_ASSETS_PATH("symmetric_cipher/idea.txt")},
    {"7-RC2", NULL, APP_ASSETS_PATH("symmetric_cipher/rc2.txt")},
    {"8-RC4", NULL, APP_ASSETS_PATH("symmetric_cipher/rc4.txt")},
    {"9-RC5", NULL, APP_ASSETS_PATH("symmetric_cipher/rc5.txt")},
    {"10-RC6", NULL, APP_ASSETS_PATH("symmetric_cipher/rc6.txt")},
    {"11-Serpent", NULL, APP_ASSETS_PATH("symmetric_cipher/serpent.txt")},
    {"12-SM4", NULL, APP_ASSETS_PATH("symmetric_cipher/sm4.txt")},
    {"13-Twofish", NULL, APP_ASSETS_PATH("symmetric_cipher/twofish.txt")},
    {"14-Triple DES", NULL, APP_ASSETS_PATH("symmetric_cipher/des3.txt")},
};

// Topic public_key[] = {
//     {"1-DSA", NULL, APP_ASSETS_PATH("development.txt")},
//     {"2-RSA", NULL, APP_ASSETS_PATH("development.txt")},
// };

// Topic key_agreement[] = {
//     {"1-DH", NULL, APP_ASSETS_PATH("development.txt")},
// };

// Topic certificates[] = {
//     {"1-X.509", NULL, APP_ASSETS_PATH("development.txt")},
//     {"2-X.509v3", NULL, APP_ASSETS_PATH("development.txt")},
// };

// Topic hash_functions[] = {
//     {"1-MD2", NULL, APP_ASSETS_PATH("development.txt")},
//     {"2-MD4", NULL, APP_ASSETS_PATH("development.txt")},
//     {"3-MD5", NULL, APP_ASSETS_PATH("development.txt")},
//     {"4-MDC2", NULL, APP_ASSETS_PATH("development.txt")},
//     {"5-RIPEMD", NULL, APP_ASSETS_PATH("development.txt")},
//     {"6-SHA", NULL, APP_ASSETS_PATH("development.txt")},
// };

// Topic authentication_codes[] = {
//     {"1-HMAC", NULL, APP_ASSETS_PATH("development.txt")},
// };

// Topic auxiliary_functions[] = {
//     {"1-ERR", NULL, APP_ASSETS_PATH("development.txt")},
//     {"2-Threads", NULL, APP_ASSETS_PATH("development.txt")},
//     {"3-RAND", NULL, APP_ASSETS_PATH("development.txt")},

// };

// Topic data_encoding[] = {
//     {"1-ASN1", NULL, APP_ASSETS_PATH("development.txt")},
//     {"2-EVP", NULL, APP_ASSETS_PATH("development.txt")},
//     {"3-PEM", NULL, APP_ASSETS_PATH("development.txt")},
//     {"4-PKCS7", NULL, APP_ASSETS_PATH("development.txt")},
//     {"5-PKCS12", NULL, APP_ASSETS_PATH("development.txt")},
// };

// Topic input_output[] = {
//     {"1-BIO", NULL, APP_ASSETS_PATH("development.txt")},
// };

// Topic internal_functions[] = {
//     {"1-BN", NULL, APP_ASSETS_PATH("development.txt")},
//     {"2-Buffer", NULL, APP_ASSETS_PATH("development.txt")},
//     {"3-EC", NULL, APP_ASSETS_PATH("development.txt")},
//     {"4-LHASH", NULL, APP_ASSETS_PATH("development.txt")},
//     {"5-Objects", NULL, APP_ASSETS_PATH("development.txt")},
//     {"6-Stack", NULL, APP_ASSETS_PATH("development.txt")},
//     {"7-TXT_DB", NULL, APP_ASSETS_PATH("development.txt")},
// };

Topic about[] = {
    // {"Algorithms", NULL, APP_ASSETS_PATH("about/algorithms.txt")},
    {"GitHub", NULL, APP_ASSETS_PATH("about/github.txt")},
};

Chapter chapters[] = {
    {"1) Symmetric Cipher", symmetric_cipher, sizeof(symmetric_cipher) / sizeof(Topic)},
    // {"2-Public Key", public_key, sizeof(public_key) / sizeof(Topic)},
    // {"3-Key Agreement", key_agreement, sizeof(key_agreement) / sizeof(Topic)},
    // {"4-Certificates", certificates, sizeof(certificates) / sizeof(Topic)},
    // {"5-Hash Functions", hash_functions, sizeof(hash_functions) / sizeof(Topic)},
    // {"6-Authentication Codes", authentication_codes, sizeof(authentication_codes) / sizeof(Topic)},
    // {"7-Auxiliary Functions", auxiliary_functions, sizeof(auxiliary_functions) / sizeof(Topic)},
    // {"8-Input/Output", input_output, sizeof(input_output) / sizeof(Topic)},
    // {"9-Data Encoding", data_encoding, sizeof(data_encoding) / sizeof(Topic)},
    // {"10-Internal Functions", internal_functions, sizeof(internal_functions) / sizeof(Topic)},
    {"11) About", about, sizeof(about) / sizeof(Topic)},
};

const size_t number_of_chapters = sizeof(chapters) / sizeof(Chapter);