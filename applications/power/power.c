#include "power_i.h"
#include "power_cli.h"
#include "power_views.h"

#include <furi.h>
#include <furi-hal.h>
#include <gui/gui.h>
#include <gui/icon_animation.h>
#include <gui/view_port.h>
#include <gui/view.h>
#include <assets_icons.h>
#include <stm32wbxx.h>

#include <notification/notification-messages.h>
#include <applications/bt/bt_service/bt.h>

#define POWER_OFF_TIMEOUT 30

void power_draw_battery_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    Power* power = context;
    canvas_draw_icon(canvas, 0, 0, &I_Battery_26x8);
    with_view_model(
        power->info_view, (PowerInfoModel * model) {
            canvas_draw_box(canvas, 2, 2, (float)model->charge / 100 * 20, 4);
            return false;
        });
}

uint32_t power_info_back_callback(void* context) {
    return VIEW_NONE;
}

void power_menu_off_callback(void* context) {
    Power* power = context;
    power_off(power);
}

void power_menu_reset_dialog_result(DialogResult result, void* context) {
    Power* power = context;
    if(result == DialogResultLeft) {
        power_reboot(power, PowerBootModeDfu);
    } else if(result == DialogResultRight) {
        power_reboot(power, PowerBootModeNormal);
    } else if(result == DialogResultBack) {
        view_dispatcher_switch_to_view(power->view_dispatcher, VIEW_NONE);
    }
}

void power_menu_reset_callback(void* context) {
    Power* power = context;
    dialog_set_result_callback(power->dialog, power_menu_reset_dialog_result);
    dialog_set_header_text(power->dialog, "Reboot type");
    dialog_set_text(power->dialog, "Reboot where?");
    dialog_set_left_button_text(power->dialog, "DFU");
    dialog_set_right_button_text(power->dialog, "OS");
    view_dispatcher_switch_to_view(power->view_dispatcher, PowerViewDialog);
}

void power_menu_enable_otg_callback(void* context) {
    furi_hal_power_enable_otg();
}

void power_menu_disable_otg_callback(void* context) {
    furi_hal_power_disable_otg();
}

void power_menu_info_callback(void* context) {
    Power* power = context;
    view_dispatcher_switch_to_view(power->view_dispatcher, PowerViewInfo);
}

Power* power_alloc() {
    Power* power = furi_alloc(sizeof(Power));

    power->state = PowerStateNotCharging;

    power->menu_vm = furi_record_open("menu");

    power->cli = furi_record_open("cli");
    power_cli_init(power->cli, power);

    power->bt = furi_record_open("bt");

    power->info_mtx = osMutexNew(NULL);

    power->menu = menu_item_alloc_menu("Power", icon_animation_alloc(&A_Power_14));
    menu_item_subitem_add(
        power->menu, menu_item_alloc_function("Off", NULL, power_menu_off_callback, power));
    menu_item_subitem_add(
        power->menu, menu_item_alloc_function("Reboot", NULL, power_menu_reset_callback, power));
    menu_item_subitem_add(
        power->menu,
        menu_item_alloc_function("Enable OTG", NULL, power_menu_enable_otg_callback, power));
    menu_item_subitem_add(
        power->menu,
        menu_item_alloc_function("Disable OTG", NULL, power_menu_disable_otg_callback, power));
    menu_item_subitem_add(
        power->menu, menu_item_alloc_function("Info", NULL, power_menu_info_callback, power));

    power->view_dispatcher = view_dispatcher_alloc();
    power->info_view = view_alloc();
    view_allocate_model(power->info_view, ViewModelTypeLockFree, sizeof(PowerInfoModel));
    view_set_draw_callback(power->info_view, power_info_draw_callback);
    view_set_previous_callback(power->info_view, power_info_back_callback);
    view_dispatcher_add_view(power->view_dispatcher, PowerViewInfo, power->info_view);

    power->off_view = view_alloc();
    view_allocate_model(power->off_view, ViewModelTypeLockFree, sizeof(PowerOffModel));
    view_set_draw_callback(power->off_view, power_off_draw_callback);
    view_dispatcher_add_view(power->view_dispatcher, PowerViewOff, power->off_view);

    power->disconnect_view = view_alloc();
    view_set_draw_callback(power->disconnect_view, power_disconnect_draw_callback);
    view_dispatcher_add_view(power->view_dispatcher, PowerViewDisconnect, power->disconnect_view);

    power->dialog = dialog_alloc();
    dialog_set_context(power->dialog, power);
    view_dispatcher_add_view(
        power->view_dispatcher, PowerViewDialog, dialog_get_view(power->dialog));

    power->battery_view_port = view_port_alloc();

    view_port_set_width(power->battery_view_port, icon_get_width(&I_Battery_26x8));
    view_port_draw_callback_set(power->battery_view_port, power_draw_battery_callback, power);
    return power;
}

void power_free(Power* power) {
    furi_assert(power);
    free(power);
}

static void power_charging_indication_handler(Power* power, NotificationApp* notifications) {
    if(furi_hal_power_is_charging()) {
        if(furi_hal_power_get_pct() == 100) {
            if(power->state != PowerStateCharged) {
                notification_internal_message(notifications, &sequence_charged);
                power->state = PowerStateCharged;
            }
        } else {
            if(power->state != PowerStateCharging) {
                notification_internal_message(notifications, &sequence_charging);
                power->state = PowerStateCharging;
            }
        }
    }

    if(!furi_hal_power_is_charging()) {
        if(power->state != PowerStateNotCharging) {
            notification_internal_message(notifications, &sequence_not_charging);
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

    NotificationApp* notifications = furi_record_open("notification");
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, power->battery_view_port, GuiLayerStatusBarRight);
    view_dispatcher_attach_to_gui(power->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    with_value_mutex(
        power->menu_vm, (Menu * menu) { menu_item_add(menu, power->menu); });

    furi_record_create("power", power);
    uint8_t battery_level = 0;
    uint8_t battery_level_prev = 0;
    while(1) {
        bool battery_low = false;

        // with_view_model(
        //     power->info_view, (PowerInfoModel * model) {
        //         model->charge = furi_hal_power_get_pct();
        //         battery_level = model->charge;
        //         model->health = furi_hal_power_get_bat_health_pct();
        //         model->capacity_remaining = furi_hal_power_get_battery_remaining_capacity();
        //         model->capacity_full = furi_hal_power_get_battery_full_capacity();
        //         model->current_charger = furi_hal_power_get_battery_current(FuriHalPowerICCharger);
        //         model->current_gauge = furi_hal_power_get_battery_current(FuriHalPowerICFuelGauge);
        //         model->voltage_charger = furi_hal_power_get_battery_voltage(FuriHalPowerICCharger);
        //         model->voltage_gauge = furi_hal_power_get_battery_voltage(FuriHalPowerICFuelGauge);
        //         model->voltage_vbus = furi_hal_power_get_usb_voltage();
        //         model->temperature_charger =
        //             furi_hal_power_get_battery_temperature(FuriHalPowerICCharger);
        //         model->temperature_gauge =
        //             furi_hal_power_get_battery_temperature(FuriHalPowerICFuelGauge);

        //         if(model->charge == 0 && model->voltage_vbus < 4.0f) {
        //             battery_low = true;
        //         }

        //         return true;
        //     });
        osMutexAcquire(power->info_mtx, osWaitForever);
        power_update_info(power);
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

        power_charging_indication_handler(power, notifications);

        if(battery_level_prev != battery_level) {
            battery_level_prev = battery_level;
            bt_update_battery_level(power->bt, battery_level);
        }

        view_port_update(power->battery_view_port);

        osDelay(1024);
    }

    return 0;
}
