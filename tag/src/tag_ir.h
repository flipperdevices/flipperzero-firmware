#ifndef TAG_IR_HEADERS
#define TAG_IR_HEADERS

#include <stdlib.h>
#include <string.h>
#include <core/check.h>
#include <lib/infrared/worker/infrared_transmit.h>
#include <lib/infrared/worker/infrared_worker.h>

#include "tag_app.h"

typedef enum { Uninitialised, Ready, Listening, Sending } TagIrMode;

TagIrMode tag_ir_mode_get();
void tag_ir_init(InfraredProtocol ir_protocol, int ir_repeats);
void tag_ir_rx_start(InfraredWorkerReceivedSignalCallback callback, void* context);
void tag_ir_rx_stop();
void tag_ir_destroy();

InfraredMessage* tag_ir_create_message(uint32_t address, uint32_t command);
void tag_ir_free_message(InfraredMessage* message);
void tag_ir_send(InfraredMessage* msg);

/** 
 * Validate the message address and command.
 * Borrowed from: https://github.com/Hong5489/ir_remote/blob/main/infrared_signal.c#L27 
 */
bool tag_ir_message_valid(InfraredMessage* message);

#endif
