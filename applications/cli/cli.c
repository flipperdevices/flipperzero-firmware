#include "cli_i.h"

#include <flipper.h>
#include <flipper_v2.h>

#include <api-hal-vcp.h>

void cli_add_command(CliCommand command) {

}

Cli * cli_alloc() {
    Cli *cli = furi_alloc(sizeof(Cli));
    CliCommandArray_init(cli->commands);
    cli_reset_state(cli);
    return cli;
}

void cli_free(Cli* cli) {
    free(cli);
}

void cli_reset_state(Cli* cli) {
    string_clear(cli->line);
    string_init(cli->line);
    cli->caret = 0;
    cli->state = CliStateEmpty;
}

void cli_putc(char c) {
    api_hal_vcp_tx((uint8_t *)&c, 1);
}

void cli_print(char* str) {
    api_hal_vcp_tx((uint8_t *)str, strlen(str));
}

void cli_motd() {
    cli_print(
        "Flipper console. "
        "Build date:" BUILD_DATE ". "
        "Git Commit:" GIT_COMMIT ". "
        "Git Branch:" GIT_BRANCH ". "
        "Commit Number:" GIT_BRANCH_NUM "."
    );
}

void cli_nl() {
    cli_print("\r\n");
}

void cli_prompt() {
    cli_print("[>_<]: ");
}

void cli_process_input(Cli* cli) {
    char c;
    size_t r = api_hal_vcp_rx((uint8_t*)&c, 1);

    if (r == 0) {
        cli_reset_state(cli);
    }

    if (c == CliSymbolAsciiTab) {
        cli_putc(CliSymbolAsciiBell);
    } else if (c == CliSymbolAsciiEsc) {
        cli_reset_state(cli);
        cli_nl();
        cli_motd();
        cli_nl();
        cli_prompt();
        cli_putc(CliSymbolAsciiBell);
    } else if (c == CliSymbolAsciiBackspace || c == CliSymbolAsciiDel) {
        if (cli->caret > 0) {
            cli->caret--;
            string_left(cli->line, cli->caret);
            cli_putc(CliSymbolAsciiBackspace);
            cli_putc(CliSymbolAsciiSpace);
            cli_putc(CliSymbolAsciiBackspace);
        } else {
            cli_putc(CliSymbolAsciiBell);
        }
    } else if (c == CliSymbolAsciiCR) {
        cli_nl();
        cli_prompt();
    } else if (c >= 0x20 && c < 0x7F) {
        string_push_back(cli->line, c);
        cli_putc(c);
        cli->caret++;
    }
}

void cli_task(void* p) {
    Cli* cli = cli_alloc();

    furiac_ready();
    cli_motd();
    while (1) {
        cli_process_input(cli);
    }
}
