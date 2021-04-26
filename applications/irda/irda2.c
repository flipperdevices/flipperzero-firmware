#include <furi.h>
#include <api-hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <cli/cli.h>

#include "irda_nec.h"
#include "irda_samsung.h"
#include "irda_protocols.h"
#include "irda-decoder/irda-decoder.h"


void print_m(void);

int32_t irda2(void* p) {
    // setup irda rx timer
    tim_irda_rx_init();

    for (;;) {
        print_m();
        delay(1000);
    }
}



int32_t irda3(void* p) {
    api_hal_tim_irda_isr_set_callback(timer_callback);

    while(1) {

    }
}


void timer_callback(TimerIRQ source)
{
    osMessageQueueId_t event_queue = (osMessageQueueId_t)comp_ctx;
    bool got_msg = false;

    source
    got_msg = irda_decode(decoder, TimerIRQRisingEdge);

    osMessageQueuePut(event_queue, &event, 0, 0);
}






#if 0
    osMessageQueueId_t event_queue = osMessageQueueNew(32, sizeof(AppEvent), NULL);

    State _state;
    uint8_t mode_count = sizeof(modes) / sizeof(modes[0]);
    uint8_t duty_cycles_count = sizeof(duty_cycles) / sizeof(duty_cycles[0]);

    _state.carrier_duty_cycle_id = duty_cycles_count - 2;
    _state.carrier_freq = 36000;
    _state.mode_id = 0;
    _state.packet_id = 0;

    IrDAApp irda_app;
    irda_app.cli = furi_record_open("cli");
    irda_app.cli_ir_rx_queue = osMessageQueueNew(1, sizeof(IrDAPacket), NULL);
    irda_app.cli_cmd_is_active = false;

    for(uint8_t i = 0; i < IRDA_PACKET_COUNT; i++) {
        init_packet(&_state, i, IRDA_UNKNOWN, 0, 0);
    }

    init_packet(&_state, 0, IRDA_NEC, 0xFF00, 0x11);
    init_packet(&_state, 1, IRDA_NEC, 0xF708, 0x59);
    init_packet(&_state, 2, IRDA_NEC, 0xFF00, 0x10);
    init_packet(&_state, 3, IRDA_NEC, 0xFF00, 0x15);
    init_packet(&_state, 4, IRDA_NEC, 0xFF00, 0x25);
    init_packet(&_state, 5, IRDA_SAMSUNG, 0xE0E, 0xF30C);
    init_packet(&_state, 6, IRDA_SAMSUNG, 0xE0E, 0xF40D);
    init_packet(&_state, 7, IRDA_SAMSUNG, 0xE0E, 0xF50E);

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, &_state, sizeof(State))) {
        printf("cannot create mutex\r\n");
        return 255;
    }

    ViewPort* view_port = view_port_alloc();

    view_port_draw_callback_set(view_port, render_callback, &state_mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    cli_add_command(irda_app.cli, "ir_rx", irda_cli_cmd_rx, &irda_app);
    cli_add_command(irda_app.cli, "ir_tx", irda_cli_cmd_tx, &state_mutex);

    // Open GUI and register view_port
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // setup irda rx timer
    tim_irda_rx_init();

    // add timer capture interrupt
    api_interrupt_add(irda_timer_capture_callback, InterruptTypeTimerCapture, event_queue);

    IrDADecoder* decoder = alloc_decoder();

    AppEvent event;
    while(1) {
        osStatus_t event_status = osMessageQueueGet(event_queue, &event, NULL, 500);

        if(event_status == osOK) {
            if(event.type == EventTypeKey) {
                State* state = (State*)acquire_mutex_block(&state_mutex);

                // press events
                if(event.value.input.type == InputTypeShort &&
                   event.value.input.key == InputKeyBack) {
                    api_interrupt_remove(irda_timer_capture_callback, InterruptTypeTimerCapture);
                    release_mutex(&state_mutex, state);

                    // remove all view_ports create by app
                    gui_remove_view_port(gui, view_port);
                    view_port_free(view_port);

                    // free decoder
                    free_decoder(decoder);

                    delete_mutex(&state_mutex);
                    osMessageQueueDelete(event_queue);
                    osMessageQueueDelete(irda_app.cli_ir_rx_queue);
                    cli_delete_command(irda_app.cli, "ir_rx");
                    cli_delete_command(irda_app.cli, "ir_tx");
                    furi_record_close("cli");

                    // exit
                    return 0;
                }

                if(event.value.input.type == InputTypeShort &&
                   event.value.input.key == InputKeyLeft) {
                    if(state->mode_id > 0) {
                        state->mode_id--;
                    }
                }

                if(event.value.input.type == InputTypeShort &&
                   event.value.input.key == InputKeyRight) {
                    if(state->mode_id < (mode_count - 1)) {
                        state->mode_id++;
                    }
                }

                modes[state->mode_id].input(&event, state);

                release_mutex(&state_mutex, state);
                view_port_update(view_port);

            } else if(event.type == EventTypeRX) {
                IrDADecoderOutputData out;
                const uint8_t out_data_length = 4;
                uint8_t out_data[out_data_length];

                out.data_length = out_data_length;
                out.data = out_data;

                api_hal_light_set(LightRed, event.value.rx.edge ? 0x00 : 0xFF);

                bool decoded =
                    process_decoder(decoder, event.value.rx.edge, &event.value.rx.lasted, 1, &out);

                if(decoded) {
                    // save only if we in packet mode
                    State* state = (State*)acquire_mutex_block(&state_mutex);
                    IrDAPacket packet;
                    packet.protocol = IRDA_NEC;
                    packet.address = out_data[1] << 8 | out_data[0];
                    packet.command = out_data[2];

                    if(state->mode_id == 1) {
                        if(out.protocol == IRDA_NEC) {
                            printf("P=NEC ");
                            printf("A=0x%02X%02X ", out_data[1], out_data[0]);
                            printf("C=0x%02X ", out_data[2]);
                            if(out.flags & IRDA_REPEAT) {
                                printf("R");
                            }
                            printf("\r\n");
                            // Save packet to state
                            memcpy(
                                &(state->packets[state->packet_id]), &packet, sizeof(IrDAPacket));
                        } else {
                            printf("Unknown protocol\r\n");
                        }
                    }
                    if(irda_app.cli_cmd_is_active) {
                        // Send decoded packet to cli
                        osMessageQueuePut(irda_app.cli_ir_rx_queue, &packet, 0, 0);
                    }

                    release_mutex(&state_mutex, state);
                    view_port_update(view_port);

                    // blink anyway
                    api_hal_light_set(LightGreen, 0xFF);
                    api_hal_light_set(LightGreen, 0x00);
                }
            }

        } else {
            // event timeout
        }
    }
#endif
