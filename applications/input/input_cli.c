#include <cli/cli.h>
#include "input.h"

static void input_dump_events_callback(const void* value, void* ctx) {
    furi_assert(value);
    furi_assert(ctx);
    osMessageQueueId_t input_queue = ctx;
    osMessageQueuePut(input_queue, value, 0, osWaitForever);
}

static void input_dump(Cli* cli, string_t args, void* context) {
    osMessageQueueId_t input_queue = osMessageQueueNew(8, sizeof(InputEvent), NULL);
    FuriPubSub* input_events = furi_record_open("input_events");
    furi_check(input_events);
    FuriPubSubSubscription* input_subscription =
        furi_pubsub_subscribe(input_events, input_dump_events_callback, input_queue);

    bool stop = false;
    InputEvent input_event;
    while(!stop) {
        if(osMessageQueueGet(input_queue, &input_event, NULL, 100) == osOK) {
            printf(
                "key: %s type: %s\r\n",
                input_get_key_name(input_event.key),
                input_get_type_name(input_event.type));
        }

        if(cli_cmd_interrupt_received(cli)) {
            stop = true;
        }
    }

    furi_pubsub_unsubscribe(input_events, input_subscription);
    furi_record_close("input_events");
    osMessageQueueDelete(input_queue);
}

void input_cli_init() {
    Cli* cli = furi_record_open("cli");
    cli_add_command(cli, "input_dump", CliCommandFlagParallelSafe, input_dump, NULL);
    furi_record_close("cli");
}