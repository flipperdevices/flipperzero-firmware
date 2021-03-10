#include "bt_views.h"

void bt_view_test_tone_tx_draw(Canvas* canvas, void* model) {
    BtViewTestToneTxModel* m = model;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 12, "Performing continous TX test");
    if(m->type == BtStatusToneTx) {
        canvas_draw_str(canvas, 0, 24, "Manual control mode");
    } else {
        canvas_draw_str(canvas, 0, 24, "Hopping mode");
    }
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Channel:%d MHz", m->channel * 2 + 2402);
    canvas_draw_str(canvas, 0, 36, buffer);
    snprintf(buffer, sizeof(buffer), "Power:%d dB", m->power - BtTestPower0dB);
    canvas_draw_str(canvas, 0, 48, buffer);
}

void bt_view_test_tone_rx_draw(Canvas* canvas, void* model) {
    BtViewTestRxModel* m = model;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 12, "Performing continous RX test");
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Channel:%d MHz", m->channel * 2 + 2402);
    canvas_draw_str(canvas, 0, 24, buffer);
}

void bt_view_test_packet_tx_draw(Canvas* canvas, void* model) {
    BtViewTestPacketTxModel* m = model;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 12, "Packets send TX test");
    if(m->type == BtStatusPacketSetup) {
        canvas_draw_str(canvas, 0, 24, "Setup parameters");
        canvas_draw_str(canvas, 0, 36, "Press OK to send packets");
    } else {
        canvas_draw_str(canvas, 0, 24, "Sending packets");
        canvas_draw_str(canvas, 0, 36, "Packets parameters:");
    }
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Channel:%d MHz", m->channel * 2 + 2402);
    canvas_draw_str(canvas, 0, 48, buffer);
    snprintf(buffer, sizeof(buffer), "Daterate:%d Mbps", m->daterate);
    canvas_draw_str(canvas, 0, 60, buffer);
}

void bt_view_app_draw(Canvas* canvas, void* model) {
    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 12, "Start BLE app");
}

BtTestChannel bt_switch_channel(InputEvent* event, BtTestChannel inst_chan) {
    if(event->key == InputKeyRight) {
        if(inst_chan == BtTestChannel2402) {
            return BtTestChannel2440;
        } else if(inst_chan == BtTestChannel2440) {
            return BtTestChannel2480;
        } else {
            return BtTestChannel2402;
        }
    } else if(event->key == InputKeyLeft) {
        if(inst_chan == BtTestChannel2402) {
            return BtTestChannel2480;
        } else if(inst_chan == BtTestChannel2480) {
            return BtTestChannel2440;
        } else {
            return BtTestChannel2402;
        }
    }
    return BtTestChannel2402;
}

bool bt_view_test_tone_tx_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);
    Bt* bt = context;
    if(event->type == InputTypeShort) {
        if(event->key == InputKeyBack) {
            if(osTimerIsRunning(bt->hopping_mode_timer)) {
                osTimerStop(bt->hopping_mode_timer);
            }
            BtMessage m = {.type = BtMessageTypeStopTestToneTx};
            osMessageQueuePut(bt->message_queue, &m, 0, osWaitForever);
            return false;
        } else {
            if(event->key == InputKeyRight || event->key == InputKeyLeft) {
                bt->state.param.channel = bt_switch_channel(event, bt->state.param.channel);
            } else if(event->key == InputKeyUp) {
                if(bt->state.param.power < BtTestPower6dB) {
                    bt->state.param.power += 2;
                }
            } else if(event->key == InputKeyDown) {
                if(bt->state.param.power > BtTestPower0dB) {
                    bt->state.param.power -= 2;
                }
            } else if(event->key == InputKeyOk) {
                if(bt->state.type == BtStatusToneTx) {
                    bt->state.type = BtStatusHoppingTx;
                    osTimerStart(bt->hopping_mode_timer, 2000);
                } else {
                    bt->state.type = BtStatusToneTx;
                    osTimerStop(bt->hopping_mode_timer);
                }
            }
            BtMessage m = {
                .type = BtMessageTypeSrartTestToneTx,
                .param.channel = bt->state.param.channel,
                .param.power = bt->state.param.power};
            osMessageQueuePut(bt->message_queue, &m, 0, osWaitForever);
            return true;
        }
    }
    return false;
}

bool bt_view_test_tone_rx_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);
    Bt* bt = context;
    if(event->type == InputTypeShort) {
        if(event->key == InputKeyRight || event->key == InputKeyLeft) {
            bt->state.param.channel = bt_switch_channel(event, bt->state.param.channel);
            BtMessage m = {
                .type = BtMessageTypeStartTestRx, .param.channel = bt->state.param.channel};
            osMessageQueuePut(bt->message_queue, &m, 0, osWaitForever);
            return true;
        } else if(event->key == InputKeyBack) {
            BtMessage m = {.type = BtMessageTypeStopTestRx};
            osMessageQueuePut(bt->message_queue, &m, 0, osWaitForever);
            return false;
        } else {
            return false;
        }
    }
    return false;
}

bool bt_view_test_packet_tx_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);
    Bt* bt = context;
    if(event->type == InputTypeShort) {
        if(event->key < InputKeyOk) {
            // Process InputKeyUp, InputKeyDown, InputKeyLeft, InputKeyRight
            if(event->key == InputKeyRight || event->key == InputKeyLeft) {
                bt->state.param.channel = bt_switch_channel(event, bt->state.param.channel);
            } else if(event->key == InputKeyUp) {
                if(bt->state.param.daterate < BtTestDateRate2M) {
                    bt->state.param.daterate += 1;
                }
            } else if(event->key == InputKeyDown) {
                if(bt->state.param.daterate > BtTestDateRate1M) {
                    bt->state.param.daterate -= 1;
                }
            }
            bt->state.type = BtStatusPacketSetup;
            BtMessage m = {
                .type = BtMessageTypeSetupTestPacketTx,
                .param.channel = bt->state.param.channel,
                .param.daterate = bt->state.param.daterate,
            };
            osMessageQueuePut(bt->message_queue, &m, 0, osWaitForever);
            return true;
        } else if(event->key == InputKeyOk) {
            bt->state.type = BtStatusPacketTx;
            BtMessage m = {
                .type = BtMessageTypeStartTestPacketTx,
                .param.channel = bt->state.param.channel,
                .param.daterate = bt->state.param.daterate,
            };
            osMessageQueuePut(bt->message_queue, &m, 0, osWaitForever);
            return true;
        } else if(event->key == InputKeyBack) {
            BtMessage m = {
                .type = BtMessageTypeStopTestPacketTx,
            };
            osMessageQueuePut(bt->message_queue, &m, 0, osWaitForever);
            return false;
        }
    }
    return false;
}
