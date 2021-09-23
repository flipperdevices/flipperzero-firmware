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
    power->info_mtx = osMutexNew(NULL);

    // Gui
    power->view_dispatcher = view_dispatcher_alloc();

    power->off_view = view_alloc();
    view_allocate_model(power->off_view, ViewModelTypeLockFree, sizeof(PowerOffModel));
    view_set_draw_callback(power->off_view, power_off_draw_callback);
    view_dispatcher_add_view(power->view_dispatcher, PowerViewOff, power->off_view);
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
        if(furi_hal_power_get_pct() == 100) {
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
    }

    if(!furi_hal_power_is_charging()) {
        if(power->state != PowerStateNotCharging) {
            notification_internal_message(power->notification, &sequence_not_charging);
            power->state = PowerStateNotCharging;
        }
    }
}

static void power_update_info(Power* power) {
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
}

int32_t power_srv(void* p) {
    (void)p;
    Power* power = power_alloc();
    furi_record_create("power", power);

    uint8_t battery_level = 0;
    uint8_t battery_level_prev = 0;
    while(1) {
        bool battery_low = false;

        osMutexAcquire(power->info_mtx, osWaitForever);
        power_update_info(power);
        battery_level = power->info.charge;
        if((power->info.charge == 0) && (power->info.voltage_vbus < 4.0f)) {
            battery_low = true;
        }
        osMutexRelease(power->info_mtx);

        with_view_model(
            power->off_view, (PowerOffModel * model) {
                if(battery_low) {
                    if(model->poweroff_tick == 0) {
                        model->poweroff_tick =
                            osKernelGetTickCount() + osKernelGetTickFreq() * POWER_OFF_TIMEOUT;
                    } else {
                        if(osKernelGetTickCount() > model->poweroff_tick) {
                            power_off(power);
                        }
                    }
                } else {
                    model->poweroff_tick = 0;
                }

                if(model->battery_low != battery_low) {
                    model->battery_low = battery_low;
                    view_dispatcher_switch_to_view(
                        power->view_dispatcher, battery_low ? PowerViewOff : VIEW_NONE);
                }
                return true;
            });

        power_charging_indication_handler(power);

        if(battery_level_prev != battery_level) {
            battery_level_prev = battery_level;
            bt_update_battery_level(power->bt, battery_level);
        }

        view_port_update(power->battery_view_port);

        osDelay(1024);
    }

    return 0;
}
