#include "details_output_formatter_table.h"
#include <inttypes.h>
#include "../../../../cli_helpers.h"

void details_output_formatter_print_header_table() {
    TOTP_CLI_PRINTF("+----------------------+-------------------------------+\r\n");
    TOTP_CLI_PRINTF("| %-20s | %-29s |\r\n", "Property", "Value");
    TOTP_CLI_PRINTF("+----------------------+-------------------------------+\r\n");
}

void details_output_formatter_print_footer_table() {
    TOTP_CLI_PRINTF("+----------------------+-------------------------------+\r\n");
}

void details_output_formatter_print_automation_feature_table(
    const char* key,
    const char* feature,
    bool* header_printed) {
    TOTP_CLI_PRINTF("| %-20s | %-29.29s |\r\n", *header_printed ? "" : key, feature);
    *header_printed = true;
}

void details_output_formatter_print_cstr_table(const char* key, const char* value) {
    TOTP_CLI_PRINTF("| %-20s | %-29.29s |\r\n", key, value);
}

void details_output_formatter_print_uint8t_table(const char* key, uint8_t value) {
    TOTP_CLI_PRINTF("| %-20s | %-29" PRIu8 " |\r\n", key, value);
}

void details_output_formatter_print_sizet_table(const char* key, size_t value) {
    TOTP_CLI_PRINTF("| %-20s | %-29" PRIu16 " |\r\n", key, value);
}

void details_output_formatter_print_uint64t_table(const char* key, uint64_t value) {
    TOTP_CLI_PRINTF("| %-20s | %-29" PRIu64 " |\r\n", key, value);
}
