#pragma once
#include <furi.h>
#include <nfc/nfc.h>
#include "nfc_cli_command_base.h"

typedef struct NfcCliProcessorContext NfcCliProcessorContext;

NfcCliProcessorContext* nfc_cli_command_processor_alloc(Nfc* nfc);
void nfc_cli_command_processor_free(NfcCliProcessorContext* instance);

/**
 * @brief Stop and free the action context left by the previous command.
 *
 * Commands that skip the processor and drive furi_hal_nfc directly MUST call
 * this first: an earlier action (e.g. raw -k) may still own a running poller
 * and, with it, the NFC GPIO ISR, timers and HAL event thread.
 */
void nfc_cli_command_processor_release_action(NfcCliProcessorContext* instance);

void nfc_cli_command_processor_run(
    const NfcCliCommandDescriptor* cmd,
    PipeSide* pipe,
    FuriString* args,
    void* context);
