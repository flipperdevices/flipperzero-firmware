#include "details_output_formatter_tsv.h"
#include <inttypes.h>
#include "../../../../cli_helpers.h"

void details_output_formatter_print_header_tsv() {
    TOTP_CLI_PRINTF("%s\t%s\r\n", "Property", "Value");
}

void details_output_formatter_print_footer_tsv() {
}

void details_output_formatter_print_automation_feature_tsv(
    const char* key,
    const char* feature,
    bool* header_printed) {
    TOTP_CLI_PRINTF("%s\t%s\r\n", *header_printed ? "" : key, feature);
    *header_printed = true;
}

void details_output_formatter_print_cstr_tsv(const char* key, const char* value) {
    TOTP_CLI_PRINTF("%s\t%s\r\n", key, value);
}

void details_output_formatter_print_uint8t_tsv(const char* key, uint8_t value) {
    TOTP_CLI_PRINTF("%s\t%" PRIu8 "\r\n", key, value);
}

void details_output_formatter_print_sizet_tsv(const char* key, size_t value) {
    TOTP_CLI_PRINTF("%s\t%" PRIu16 "\r\n", key, value);
}

void details_output_formatter_print_uint64t_tsv(const char* key, uint64_t value) {
    TOTP_CLI_PRINTF("%s\t%" PRIu64 "\r\n", key, value);
}
