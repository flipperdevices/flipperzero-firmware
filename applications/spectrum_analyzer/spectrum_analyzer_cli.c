/*
 * spectrum_analyzer_cli.c
 *
 *  Created on: Dec 4, 2021
 *      Author: forthe
 */
#include "spectrum_analyzer_cli.h"
#include "spectrum_analyzer.h"
#include "spectrum_analyzer_worker.h"
#include <furi.h>
#include <furi-hal.h>


void cli_command_spectrum_analyzer(Cli* cli, string_t args, void* context) {
	uint32_t frequency_start;
	uint32_t frequency_end;
	uint32_t bandwidth;
    if(string_size(args)) {
        int ret = sscanf(
        		string_get_cstr(args), "%lu %lu %lu", &frequency_start,
				&frequency_end, &bandwidth);
        printf("Start %lu, end %lu", frequency_start, frequency_end);
        if(ret != 3) {
            cli_print_usage("spectrum_analyzer", "<Frequency_start Frequency_end bandwidth in HZ>", string_get_cstr(args));
            return;
        }
//        if(!furi_hal_subghz_is_frequency_valid(frequency)) {
//            printf(
//                "Frequency must be in " SUBGHZ_FREQUENCY_RANGE_STR " range, not %lu\r\n",
//                frequency);
//            return;

    }
    printf("Starting worker\n");
    SpectrumAnalyzerWorker* worker = spectrum_analyzer_worker_alloc();
    printf("Worker started\n");
    while(!cli_cmd_interrupt_received(cli)) {
        osDelay(250);
        FuriThreadState state = furi_thread_get_state(worker->thread);
        printf("state=%i\n\r", state);

    }
    printf("Stop worker\n");
    spectrum_analyzer_worker_free(worker);

}



void spectrum_analyzer_cli_init() {
    Cli* cli = furi_record_open("cli");

    cli_add_command(cli, "spectrum_analyzer", CliCommandFlagDefault, cli_command_spectrum_analyzer, NULL);

    furi_record_close("cli");
}
