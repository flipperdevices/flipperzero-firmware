/*
 * spectrum_analyzer_cli.c
 *
 *  Created on: Dec 4, 2021
 *      Author: forthe
 */
#include "spectrum_analyzer_cli.h"

#include <furi.h>
#include <furi-hal.h>


void cli_command_spectrum_analyzer(Cli* cli, string_t args, void* context) {
	printf("hello world!");
}




void spectrum_analyzer_cli_init() {
    Cli* cli = furi_record_open("cli");

    cli_add_command(cli, "spectrum_analyzer", CliCommandFlagDefault, cli_command_spectrum_analyzer, NULL);

    furi_record_close("cli");
}
