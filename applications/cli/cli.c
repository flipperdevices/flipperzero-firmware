#include "cli_i.h"

#include <flipper.h>
#include <flipper_v2.h>

#include <api-hal-vcp.h>

Cli* cli_alloc() {
    Cli* cli = furi_alloc(sizeof(Cli));
    CliCommandDict_init(cli->commands);
    cli_reset_state(cli);
    return cli;
}

void cli_free(Cli* cli) {
    free(cli);
}

void cli_reset_state(Cli* cli) {
    string_clear(cli->line);
    string_init(cli->line);
    cli->state = CliStateIdle;
}

void cli_putc(char c) {
    api_hal_vcp_tx((uint8_t*)&c, 1);
}

void cli_print(const char* str) {
    api_hal_vcp_tx((uint8_t*)str, strlen(str));
}

void cli_print_version() {
    cli_print("Build date:" BUILD_DATE ". "
              "Git Commit:" GIT_COMMIT ". "
              "Git Branch:" GIT_BRANCH ". "
              "Commit Number:" GIT_BRANCH_NUM ".");
}

void cli_motd() {
    cli_print("Flipper cli.\r\n");
    cli_print_version();
}

void cli_nl() {
    cli_print("\r\n");
}

void cli_prompt() {
    cli_print("\r\n>: ");
}

void cli_process_input(Cli* cli) {
    char c;
    size_t r = api_hal_vcp_rx((uint8_t*)&c, 1);

    if(r == 0) {
        cli_reset_state(cli);
    }

    if(c == CliSymbolAsciiTab) {
        cli_putc(CliSymbolAsciiBell);
    } else if(c == CliSymbolAsciiSOH) {
        cli_motd();
        cli_prompt();
    } else if(c == CliSymbolAsciiEOT) {
        cli_reset_state(cli);
    } else if(c == CliSymbolAsciiEsc) {
        cli_putc(CliSymbolAsciiBell);
    } else if(c == CliSymbolAsciiBackspace || c == CliSymbolAsciiDel) {
        size_t s = string_size(cli->line);
        if(s > 0) {
            s--;
            string_left(cli->line, s);
            cli_putc(CliSymbolAsciiBackspace);
            cli_putc(CliSymbolAsciiSpace);
            cli_putc(CliSymbolAsciiBackspace);
        } else {
            cli_putc(CliSymbolAsciiBell);
        }
    } else if(c == CliSymbolAsciiCR) {
        CliCallback* command_pp = CliCommandDict_get(cli->commands, cli->line);
        if(command_pp) {
            cli_nl();
            cli->state = CliStateRunning;
            (*command_pp)(NULL);
            cli_reset_state(cli);
            cli_prompt();
        } else {
            cli_putc(CliSymbolAsciiBell);
        }
    } else if(c >= 0x20 && c < 0x7F) {
        string_push_back(cli->line, c);
        cli_putc(c);
    }
}

void cli_add_command(Cli* cli, const char* name, CliCallback command) {
    string_t name_str;
    string_init_set_str(name_str, name);
    CliCommandDict_set_at(cli->commands, name_str, command);
}

void cli_help(char* args) {
    cli_print("Help: no help, sorry.");
}

void cli_version(char* args) {
    cli_print_version();
}

void cli_task(void* p) {
    Cli* cli = cli_alloc();

    furiac_ready();

    cli_add_command(cli, "help", cli_help);
    cli_add_command(cli, "version", cli_version);

    while(1) {
        cli_process_input(cli);
    }
}
