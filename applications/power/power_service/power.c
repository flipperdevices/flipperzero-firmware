#include "power_i.h"
#include "views/power_off.h"

#include <furi.h>
#include <furi-hal.h>
#include <gui/view_port.h>
#include <gui/view.h>

#define POWER_OFF_TIMEOUT 30

void power_draw_battery_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    Power* power = context;
    canvas_draw_icon(canvas, 0, 0, &I_Battery_26x8);
    canvas_draw_box(canvas, 2, 2, power->info.charge / 5, 4);
}

static ViewPort* power_battery_view_port_alloc(Power* power) {
    ViewPort* battery_view_port = view_port_alloc();
    view_port_set_width(battery_view_port, icon_get_width(&I_Battery_26x8));
    view_port_draw_callback_set(battery_view_port, power_draw_battery_callback, power);
    gui_add_view_port(power->gui, battery_view_port, GuiLayerStatusBarRight);
    return battery_view_port;
}

Power* power_alloc() {
    Power* power = furi_alloc(sizeof(Power));

    // Records
    power->bt = furi_record_open("bt");
    power->notification = furi_record_open("notification");
    power->gui = furi_record_open("gui");

    // State initialization
    power->state = PowerStateNotCharging;
    power->battery_low = false;
    power->power_off_timeout = POWER_OFF_TIMEOUT;
    power->info_mtx = osMutexNew(NULL);

    // Gui
    power->view_dispatcher = view_dispatcher_alloc();
    power->power_off = power_off_alloc();
    view_dispatcher_add_view(
        power->view_dispatcher, PowerViewOff, power_off_get_view(power->power_off));
    view_dispatcher_attach_to_gui(
        power->view_dispatcher, power->gui, ViewDispatcherTypeFullscreen);

    // Battery view port
    power->battery_view_port = power_battery_view_port_alloc(power);

    return power;
}

void power_free(Power* power) {
    furi_assert(power);
    free(power);
}

static void power_charging_indication_handler(Power* power) {
    if(furi_hal_power_is_charging()) {
        if(power->info.charge == 100) {
            if(power->state != PowerStateCharged) {
                notification_internal_message(power->notification, &sequence_charged);
                power->state = PowerStateCharged;
            }
        } else {
            if(power->state != PowerStateCharging) {
                notification_internal_message(power->notification, &sequence_charging);
                power->state = PowerStateCharging;
            }
        }
    } else {
        if(power->state != PowerStateNotCharging) {
            notification_internal_message(power->notification, &sequence_not_charging);
            power->state = PowerStateNotCharging;
        }
    }
}

static void power_update_info(Power* power) {
    osMutexAcquire(power->info_mtx, osWaitForever);
    PowerInfo* info = &power->info;

    info->charge = furi_hal_power_get_pct();
    info->health = furi_hal_power_get_bat_health_pct();
    info->capacity_remaining = furi_hal_power_get_battery_remaining_capacity();
    info->capacity_full = furi_hal_power_get_battery_full_capacity();
    info->current_charger = furi_hal_power_get_battery_current(FuriHalPowerICCharger);
    info->current_gauge = furi_hal_power_get_battery_current(FuriHalPowerICFuelGauge);
    info->voltage_charger = furi_hal_power_get_battery_voltage(FuriHalPowerICCharger);
    info->voltage_gauge = furi_hal_power_get_battery_voltage(FuriHalPowerICFuelGauge);
    info->voltage_vbus = furi_hal_power_get_usb_voltage();
    info->temperature_charger = furi_hal_power_get_battery_temperature(FuriHalPowerICCharger);
    info->temperature_gauge = furi_hal_power_get_battery_temperature(FuriHalPowerICFuelGauge);

    osMutexRelease(power->info_mtx);
}

static void power_check_low_battery(Power* power) {
    // Check battery charge and vbus voltage
    if((power->info.charge == 0) && (power->info.voltage_vbus < 4.0f)) {
        if(!power->battery_low) {
            view_dispatcher_switch_to_view(power->view_dispatcher, PowerViewOff);
        }
        power->battery_low = true;
    } else {
        if(power->battery_low) {
            view_dispatcher_switch_to_view(power->view_dispatcher, VIEW_NONE);
            power->power_off_timeout = POWER_OFF_TIMEOUT;
        }
        power->battery_low = false;
    }
    // If battery low, update view and switch off power after timeout
    if(power->battery_low) {
        if(power->power_off_timeout) {
            power_off_set_time_left(power->power_off, power->power_off_timeout--);
        } else {
            power_off();
        }
    }
}

int32_t power_srv(void* p) {
    (void)p;
    Power* power = power_alloc();
    furi_record_create("power", power);

    uint8_t battery_level = 0;
    uint8_t battery_level_prev = 0;
    while(1) {
        // Update data from gauge and charger
        power_update_info(power);

        battery_level = power->info.charge;

        // Check low battery level
        power_check_low_battery(power);

        // Process charging state
        power_charging_indication_handler(power);

        if(battery_level_prev != battery_level) {
            battery_level_prev = battery_level;
            bt_update_battery_level(power->bt, battery_level);
        }

        // Update battery view port
        view_port_update(power->battery_view_port);

        osDelay(1024);
    }

    return 0;
}
