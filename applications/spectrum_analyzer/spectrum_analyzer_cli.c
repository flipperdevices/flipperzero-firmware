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
	uint32_t bandwidth;
    if(string_size(args)) {
        int ret = sscanf(
        		string_get_cstr(args), "%lu %lu", &frequency_start,
				&bandwidth);
        printf("Start %lu, bw bits %lu", frequency_start, bandwidth);
        if(ret != 3) {
            cli_print_usage("spectrum_analyzer", "<Frequency_start Frequency_end bandwidth from list 812 650 541 464 406 325 270 232 203 162 135 116 102 81 68 58>", string_get_cstr(args));
            return;
        }
//        if(!furi_hal_subghz_is_frequency_valid(frequency)) {
//            printf(
//                "Frequency must be in " SUBGHZ_FREQUENCY_RANGE_STR " range, not %lu\r\n",
//                frequency);
//            return;

    }
    printf("Starting worker\r\n");
    SpectrumAnalyzerWorker* worker = spectrum_analyzer_worker_alloc();

    worker->start_freq = frequency_start;
    worker->bandwidth = bandwidth;
    spectrum_analyzer_worker_start(worker);
    printf("Worker started\n");
    while(!cli_cmd_interrupt_received(cli)) {
        osDelay(1000);
        uint8_t rssi_ind = 0;
        printf("RSSI vals: ");
        for(rssi_ind = 0; rssi_ind < DOTS_COUNT; rssi_ind++)
        {
        	printf("%u ", worker->rssi_buf[rssi_ind]);
        }
        printf("\n\r");
    }
    printf("Stop worker\n");
    spectrum_analyzer_worker_free(worker);

}



void spectrum_analyzer_cli_init() {
    Cli* cli = furi_record_open("cli");

    cli_add_command(cli, "spectrum_analyzer", CliCommandFlagDefault, cli_command_spectrum_analyzer, NULL);

    furi_record_close("cli");
}
