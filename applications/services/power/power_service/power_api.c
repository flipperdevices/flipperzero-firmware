#include "power_i.h"

void power_off(Power* power) {
    furi_check(power);

    PowerMessage msg = {
        .type = PowerMessageTypeShutdown,
    };

    furi_check(
        furi_message_queue_put(power->message_queue, &msg, FuriWaitForever) == FuriStatusOk);
}

void power_reboot(Power* power, PowerBootMode mode) {
    PowerMessage msg = {
        .type = PowerMessageTypeReboot,
        .boot_mode = mode,
    };

    furi_check(
        furi_message_queue_put(power->message_queue, &msg, FuriWaitForever) == FuriStatusOk);
}

void power_get_info(Power* power, PowerInfo* info) {
    furi_check(power);
    furi_check(info);

    PowerMessage msg = {
        .type = PowerMessageTypeGetInfo,
        .power_info = info,
        .lock = api_lock_alloc_locked(),
    };

    furi_check(
        furi_message_queue_put(power->message_queue, &msg, FuriWaitForever) == FuriStatusOk);
    api_lock_wait_unlock_and_free(msg.lock);
}

FuriPubSub* power_get_pubsub(Power* power) {
    furi_check(power);
    return power->event_pubsub;
}

bool power_is_battery_healthy(Power* power) {
    furi_check(power);

    PowerMessage msg = {
        .type = PowerMessageTypeIsBatteryHealthy,
        .lock = api_lock_alloc_locked(),
    };

    furi_check(
        furi_message_queue_put(power->message_queue, &msg, FuriWaitForever) == FuriStatusOk);
    api_lock_wait_unlock_and_free(msg.lock);

    return msg.bool_param;
}

void power_enable_low_battery_level_notification(Power* power, bool enable) {
    furi_check(power);

    PowerMessage msg = {
        .type = PowerMessageTypeIsBatteryHealthy,
        .bool_param = enable,
    };

    furi_check(
        furi_message_queue_put(power->message_queue, &msg, FuriWaitForever) == FuriStatusOk);
}
