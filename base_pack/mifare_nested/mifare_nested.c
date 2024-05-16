#include "mifare_nested_i.h"
#include <gui/elements.h>
#include <furi_hal_nfc.h>
#include <lib/drivers/st25r3916.h>

bool mifare_nested_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    MifareNested* mifare_nested = context;
    return scene_manager_handle_custom_event(mifare_nested->scene_manager, event);
}

bool mifare_nested_back_event_callback(void* context) {
    furi_assert(context);
    MifareNested* mifare_nested = context;
    return scene_manager_handle_back_event(mifare_nested->scene_manager);
}

void mifare_nested_tick_event_callback(void* context) {
    furi_assert(context);
    MifareNested* mifare_nested = context;
    scene_manager_handle_tick_event(mifare_nested->scene_manager);
}

void mifare_nested_show_loading_popup(void* context, bool show) {
    MifareNested* mifare_nested = context;

    if(show) {
        // Raise timer priority so that animations can play
        furi_timer_set_thread_priority(FuriTimerThreadPriorityElevated);
        view_dispatcher_switch_to_view(mifare_nested->view_dispatcher, MifareNestedViewLoading);
    } else {
        // Restore default timer priority
        furi_timer_set_thread_priority(FuriTimerThreadPriorityNormal);
    }
}

NestedState* collection_alloc() {
    NestedState* nested = malloc(sizeof(NestedState));
    nested->view = view_alloc();
    view_allocate_model(nested->view, ViewModelTypeLocking, sizeof(NestedAttackViewModel));
    with_view_model(
        nested->view,
        NestedAttackViewModel * model,
        {
            model->header = furi_string_alloc();
            furi_string_set(model->header, "Collecting nonces");
            model->keys_count = 0;
            model->hardnested_states = 0;
            model->lost_tag = false;
            model->calibrating = false;
            model->need_prediction = false;
            model->hardnested = false;
        },
        false);

    return nested;
}

CheckKeysState* check_keys_alloc() {
    CheckKeysState* state = malloc(sizeof(CheckKeysState));
    state->view = view_alloc();
    view_allocate_model(state->view, ViewModelTypeLocking, sizeof(CheckKeysViewModel));
    with_view_model(
        state->view,
        CheckKeysViewModel * model,
        {
            model->header = furi_string_alloc();
            furi_string_set(model->header, "Checking keys");
            model->lost_tag = false;
        },
        false);

    return state;
}

static void nested_draw_callback(Canvas* canvas, void* model) {
    NestedAttackViewModel* m = model;

    if(m->lost_tag) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 4, AlignCenter, AlignTop, "Lost the tag!");
        canvas_set_font(canvas, FontSecondary);
        elements_multiline_text_aligned(
            canvas, 64, 23, AlignCenter, AlignTop, "Make sure the tag is\npositioned correctly.");
    } else if(m->calibrating) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 4, AlignCenter, AlignTop, "Calibrating...");
        canvas_set_font(canvas, FontSecondary);
        if(!m->need_prediction) {
            elements_multiline_text_aligned(
                canvas, 64, 23, AlignCenter, AlignTop, "Don't touch or move\nFlipper/Tag!");
        } else {
            elements_multiline_text_aligned(
                canvas, 64, 18, AlignCenter, AlignTop, "Don't touch or move tag!");
            canvas_set_font(canvas, FontPrimary);
            elements_multiline_text_aligned(
                canvas, 64, 30, AlignCenter, AlignTop, "Calibration will take\nmore time");
        }
    } else if(m->hardnested) {
        char draw_str[32] = {};
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(
            canvas, 64, 2, AlignCenter, AlignTop, furi_string_get_cstr(m->header));
        canvas_set_font(canvas, FontSecondary);

        float progress =
            m->keys_count == 0 ? 0 : (float)(m->nonces_collected) / (float)(m->keys_count);

        if(progress > 1.0) {
            progress = 1.0;
        }

        elements_progress_bar(canvas, 5, 15, 120, progress);
        canvas_set_font(canvas, FontSecondary);
        snprintf(
            draw_str,
            sizeof(draw_str),
            "Nonces collected: %lu/%lu",
            m->nonces_collected,
            m->keys_count);
        canvas_draw_str_aligned(canvas, 1, 28, AlignLeft, AlignTop, draw_str);
        snprintf(draw_str, sizeof(draw_str), "States found: %lu/256", m->hardnested_states);
        canvas_draw_str_aligned(canvas, 1, 40, AlignLeft, AlignTop, draw_str);
    } else {
        char draw_str[32] = {};
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(
            canvas, 64, 2, AlignCenter, AlignTop, furi_string_get_cstr(m->header));
        canvas_set_font(canvas, FontSecondary);

        float progress =
            m->keys_count == 0 ? 0 : (float)(m->nonces_collected) / (float)(m->keys_count);

        if(progress > 1.0) {
            progress = 1.0;
        }

        elements_progress_bar(canvas, 5, 15, 120, progress);
        canvas_set_font(canvas, FontSecondary);
        snprintf(
            draw_str,
            sizeof(draw_str),
            "Nonces collected: %lu/%lu",
            m->nonces_collected,
            m->keys_count);
        canvas_draw_str_aligned(canvas, 1, 28, AlignLeft, AlignTop, draw_str);
    }

    elements_button_center(canvas, "Stop");
}

static void check_keys_draw_callback(Canvas* canvas, void* model) {
    CheckKeysViewModel* m = model;

    if(m->lost_tag) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 4, AlignCenter, AlignTop, "Lost the tag!");
        canvas_set_font(canvas, FontSecondary);
        elements_multiline_text_aligned(
            canvas, 64, 23, AlignCenter, AlignTop, "Make sure the tag is\npositioned correctly.");
    } else if(m->processing_keys) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 4, AlignCenter, AlignTop, "Processing keys...");
        canvas_set_font(canvas, FontSecondary);
        elements_multiline_text_aligned(
            canvas, 64, 23, AlignCenter, AlignTop, "Checking which keys you\nalready have...");
    } else {
        char draw_str[32] = {};
        char draw_sub_str[32] = {};
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(
            canvas, 64, 2, AlignCenter, AlignTop, furi_string_get_cstr(m->header));
        canvas_set_font(canvas, FontSecondary);

        float progress = m->keys_count == 0 ? 0 :
                                              (float)(m->keys_checked) / (float)(m->keys_count);

        if(progress > 1.0) {
            progress = 1.0;
        }

        elements_progress_bar(canvas, 5, 15, 120, progress);
        canvas_set_font(canvas, FontSecondary);
        snprintf(
            draw_str, sizeof(draw_str), "Keys checked: %lu/%lu", m->keys_checked, m->keys_count);
        canvas_draw_str_aligned(canvas, 1, 28, AlignLeft, AlignTop, draw_str);
        snprintf(
            draw_sub_str,
            sizeof(draw_sub_str),
            "Keys found: %lu/%lu",
            m->keys_found,
            m->keys_total);
        canvas_draw_str_aligned(canvas, 1, 40, AlignLeft, AlignTop, draw_sub_str);
    }

    elements_button_center(canvas, "Stop");
}

static bool nested_input_callback(InputEvent* event, void* context) {
    MifareNested* mifare_nested = context;

    bool consumed = false;

    if(event->type == InputTypeShort && (event->key == InputKeyBack || event->key == InputKeyOk)) {
        scene_manager_search_and_switch_to_previous_scene(mifare_nested->scene_manager, 0);
        consumed = true;
    }

    return consumed;
}

MifareNested* mifare_nested_alloc() {
    MifareNested* mifare_nested = malloc(sizeof(MifareNested));

    mifare_nested->worker = mifare_nested_worker_alloc();
    mifare_nested->view_dispatcher = view_dispatcher_alloc();
    mifare_nested->scene_manager =
        scene_manager_alloc(&mifare_nested_scene_handlers, mifare_nested);
    view_dispatcher_enable_queue(mifare_nested->view_dispatcher);
    view_dispatcher_set_event_callback_context(mifare_nested->view_dispatcher, mifare_nested);
    view_dispatcher_set_custom_event_callback(
        mifare_nested->view_dispatcher, mifare_nested_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        mifare_nested->view_dispatcher, mifare_nested_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        mifare_nested->view_dispatcher, mifare_nested_tick_event_callback, 100);

    // Nfc device
    mifare_nested->nfc_dev = nfc_device_alloc();

    // Open GUI record
    mifare_nested->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        mifare_nested->view_dispatcher, mifare_nested->gui, ViewDispatcherTypeFullscreen);

    // Open Notification record
    mifare_nested->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Submenu
    mifare_nested->submenu = submenu_alloc();
    view_dispatcher_add_view(
        mifare_nested->view_dispatcher,
        MifareNestedViewMenu,
        submenu_get_view(mifare_nested->submenu));

    // Popup
    mifare_nested->popup = popup_alloc();
    view_dispatcher_add_view(
        mifare_nested->view_dispatcher,
        MifareNestedViewPopup,
        popup_get_view(mifare_nested->popup));

    // Loading
    mifare_nested->loading = loading_alloc();
    view_dispatcher_add_view(
        mifare_nested->view_dispatcher,
        MifareNestedViewLoading,
        loading_get_view(mifare_nested->loading));

    // Text Input
    mifare_nested->text_input = text_input_alloc();
    view_dispatcher_add_view(
        mifare_nested->view_dispatcher,
        MifareNestedViewTextInput,
        text_input_get_view(mifare_nested->text_input));

    // Custom Widget
    mifare_nested->widget = widget_alloc();
    view_dispatcher_add_view(
        mifare_nested->view_dispatcher,
        MifareNestedViewWidget,
        widget_get_view(mifare_nested->widget));

    // Variable Item List
    mifare_nested->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        mifare_nested->view_dispatcher,
        MifareNestedViewVariableList,
        variable_item_list_get_view(mifare_nested->variable_item_list));

    // Nested attack state
    NestedState* plugin_state = collection_alloc();
    view_set_context(plugin_state->view, mifare_nested);
    mifare_nested->nested_state = plugin_state;
    view_dispatcher_add_view(
        mifare_nested->view_dispatcher, MifareNestedViewCollecting, plugin_state->view);

    // Check keys attack state
    CheckKeysState* keys_state = check_keys_alloc();
    view_set_context(keys_state->view, mifare_nested);
    mifare_nested->keys_state = keys_state;
    view_dispatcher_add_view(
        mifare_nested->view_dispatcher, MifareNestedViewCheckKeys, keys_state->view);

    KeyInfo_t* key_info = malloc(sizeof(KeyInfo_t));
    mifare_nested->keys = key_info;

    MifareNestedSettings* settings = malloc(sizeof(MifareNestedSettings));
    settings->only_hardnested = false;
    mifare_nested->settings = settings;

    view_set_draw_callback(plugin_state->view, nested_draw_callback);
    view_set_input_callback(plugin_state->view, nested_input_callback);

    view_set_draw_callback(keys_state->view, check_keys_draw_callback);
    view_set_input_callback(keys_state->view, nested_input_callback);

    mifare_nested->collecting_type = MifareNestedWorkerStateReady;
    mifare_nested->run = NestedRunIdle;

    return mifare_nested;
}

void mifare_nested_free(MifareNested* mifare_nested) {
    furi_assert(mifare_nested);

    // Nfc device
    nfc_device_free(mifare_nested->nfc_dev);

    // Submenu
    view_dispatcher_remove_view(mifare_nested->view_dispatcher, MifareNestedViewMenu);
    submenu_free(mifare_nested->submenu);

    // Popup
    view_dispatcher_remove_view(mifare_nested->view_dispatcher, MifareNestedViewPopup);
    popup_free(mifare_nested->popup);

    // Loading
    view_dispatcher_remove_view(mifare_nested->view_dispatcher, MifareNestedViewLoading);
    loading_free(mifare_nested->loading);

    // TextInput
    view_dispatcher_remove_view(mifare_nested->view_dispatcher, MifareNestedViewTextInput);
    text_input_free(mifare_nested->text_input);

    // Custom Widget
    view_dispatcher_remove_view(mifare_nested->view_dispatcher, MifareNestedViewWidget);
    widget_free(mifare_nested->widget);

    // Variable Item List
    view_dispatcher_remove_view(mifare_nested->view_dispatcher, MifareNestedViewVariableList);
    variable_item_list_free(mifare_nested->variable_item_list);

    // Nested
    view_dispatcher_remove_view(mifare_nested->view_dispatcher, MifareNestedViewCollecting);

    // Check keys
    view_dispatcher_remove_view(mifare_nested->view_dispatcher, MifareNestedViewCheckKeys);

    // Nonces states
    free(mifare_nested->nested_state);

    // Keys
    free(mifare_nested->keys);

    // Settings
    free(mifare_nested->settings);

    // Worker
    mifare_nested_worker_stop(mifare_nested->worker);
    mifare_nested_worker_free(mifare_nested->worker);

    // View Dispatcher
    view_dispatcher_free(mifare_nested->view_dispatcher);

    // Scene Manager
    scene_manager_free(mifare_nested->scene_manager);

    // GUI
    furi_record_close(RECORD_GUI);
    mifare_nested->gui = NULL;

    // Notifications
    furi_record_close(RECORD_NOTIFICATION);
    mifare_nested->notifications = NULL;

    free(mifare_nested);
}

void mifare_nested_blink_start(MifareNested* mifare_nested) {
    notification_message(mifare_nested->notifications, &mifare_nested_sequence_blink_start_blue);
}

void mifare_nested_blink_calibration_start(MifareNested* mifare_nested) {
    notification_message(
        mifare_nested->notifications, &mifare_nested_sequence_blink_start_magenta);
}

void mifare_nested_blink_nonce_collection_start(MifareNested* mifare_nested) {
    notification_message(mifare_nested->notifications, &mifare_nested_sequence_blink_start_yellow);
}

void mifare_nested_blink_stop(MifareNested* mifare_nested) {
    notification_message(mifare_nested->notifications, &mifare_nested_sequence_blink_stop);
}

void remove_new_nfc_and_init_old() {
    // Deinitialize new NFC and init legacy
    if(furi_hal_nfc_is_hal_ready() != FuriHalNfcErrorNone) {
        FURI_LOG_E("nested", "NFC chip failed to start\r\n");
        return;
    }

    furi_hal_nfc_acquire();
    furi_hal_nfc_reset_mode();
    furi_hal_nfc_low_power_mode_start();
    furi_hal_nfc_release();
    furry_hal_nfc_init();
}

void remove_old_nfc_and_init_new_back() {
    // Deinitialize NFC
    furry_hal_nfc_deinit();

    //
    furi_hal_nfc_acquire();
    furi_hal_nfc_reset_mode();
    furi_hal_nfc_low_power_mode_start();

    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    // Set default state
    st25r3916_direct_cmd(handle, ST25R3916_CMD_SET_DEFAULT);
    // Increase IO driver strength of MISO and IRQ
    st25r3916_write_reg(handle, ST25R3916_REG_IO_CONF2, ST25R3916_REG_IO_CONF2_io_drv_lvl);
    // Check chip ID
    uint8_t chip_id = 0;
    st25r3916_read_reg(handle, ST25R3916_REG_IC_IDENTITY, &chip_id);
    if((chip_id & ST25R3916_REG_IC_IDENTITY_ic_type_mask) !=
       ST25R3916_REG_IC_IDENTITY_ic_type_st25r3916) {
        furi_hal_nfc_low_power_mode_start();
        furi_hal_nfc_release();
        return;
    }
    // Clear interrupts
    st25r3916_get_irq(handle);
    // Mask all interrupts
    st25r3916_mask_irq(handle, ST25R3916_IRQ_MASK_ALL);
    // Enable interrupts
    //furi_hal_nfc_init_gpio_isr();
    // Disable internal overheat protection
    st25r3916_change_test_reg_bits(handle, 0x04, 0x10, 0x10);

    // Measure voltage
    // Set measure power supply voltage source
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_REGULATOR_CONTROL,
        ST25R3916_REG_REGULATOR_CONTROL_mpsv_mask,
        ST25R3916_REG_REGULATOR_CONTROL_mpsv_vdd);
    // Enable timer and interrupt register
    st25r3916_mask_irq(handle, ~ST25R3916_IRQ_MASK_DCT);
    st25r3916_direct_cmd(handle, ST25R3916_CMD_MEASURE_VDD);
    //furi_hal_nfc_event_wait_for_specific_irq(handle, ST25R3916_IRQ_MASK_DCT, 100);
    st25r3916_mask_irq(handle, ST25R3916_IRQ_MASK_ALL);
    uint8_t ad_res = 0;
    st25r3916_read_reg(handle, ST25R3916_REG_AD_RESULT, &ad_res);
    uint16_t mV = ((uint16_t)ad_res) * 23U;
    mV += (((((uint16_t)ad_res) * 4U) + 5U) / 10U);

    if(mV < 3600) {
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_IO_CONF2,
            ST25R3916_REG_IO_CONF2_sup3V,
            ST25R3916_REG_IO_CONF2_sup3V_3V);
    } else {
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_IO_CONF2,
            ST25R3916_REG_IO_CONF2_sup3V,
            ST25R3916_REG_IO_CONF2_sup3V_5V);
    }

    // Disable MCU CLK
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_IO_CONF1,
        ST25R3916_REG_IO_CONF1_out_cl_mask | ST25R3916_REG_IO_CONF1_lf_clk_off,
        0x07);
    // Disable MISO pull-down
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_IO_CONF2,
        ST25R3916_REG_IO_CONF2_miso_pd1 | ST25R3916_REG_IO_CONF2_miso_pd2,
        0x00);
    // Set tx driver resistance to 1 Om
    st25r3916_change_reg_bits(
        handle, ST25R3916_REG_TX_DRIVER, ST25R3916_REG_TX_DRIVER_d_res_mask, 0x00);
    // Use minimum non-overlap
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_RES_AM_MOD,
        ST25R3916_REG_RES_AM_MOD_fa3_f,
        ST25R3916_REG_RES_AM_MOD_fa3_f);

    // Set activation threashold
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_FIELD_THRESHOLD_ACTV,
        ST25R3916_REG_FIELD_THRESHOLD_ACTV_trg_mask,
        ST25R3916_REG_FIELD_THRESHOLD_ACTV_trg_105mV);
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_FIELD_THRESHOLD_ACTV,
        ST25R3916_REG_FIELD_THRESHOLD_ACTV_rfe_mask,
        ST25R3916_REG_FIELD_THRESHOLD_ACTV_rfe_105mV);
    // Set deactivation threashold
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_FIELD_THRESHOLD_DEACTV,
        ST25R3916_REG_FIELD_THRESHOLD_DEACTV_trg_mask,
        ST25R3916_REG_FIELD_THRESHOLD_DEACTV_trg_75mV);
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_FIELD_THRESHOLD_DEACTV,
        ST25R3916_REG_FIELD_THRESHOLD_DEACTV_rfe_mask,
        ST25R3916_REG_FIELD_THRESHOLD_DEACTV_rfe_75mV);
    // Enable external load modulation
    st25r3916_change_reg_bits(
        handle, ST25R3916_REG_AUX_MOD, ST25R3916_REG_AUX_MOD_lm_ext, ST25R3916_REG_AUX_MOD_lm_ext);
    // Enable internal load modulation
    st25r3916_change_reg_bits(
        handle, ST25R3916_REG_AUX_MOD, ST25R3916_REG_AUX_MOD_lm_dri, ST25R3916_REG_AUX_MOD_lm_dri);
    // Adjust FDT
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_PASSIVE_TARGET,
        ST25R3916_REG_PASSIVE_TARGET_fdel_mask,
        (5U << ST25R3916_REG_PASSIVE_TARGET_fdel_shift));
    // Reduce RFO resistance in Modulated state
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_PT_MOD,
        ST25R3916_REG_PT_MOD_ptm_res_mask | ST25R3916_REG_PT_MOD_pt_res_mask,
        0x0f);
    // Enable RX start on first 4 bits
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_EMD_SUP_CONF,
        ST25R3916_REG_EMD_SUP_CONF_rx_start_emv,
        ST25R3916_REG_EMD_SUP_CONF_rx_start_emv_on);
    // Set antena tunning
    st25r3916_change_reg_bits(handle, ST25R3916_REG_ANT_TUNE_A, 0xff, 0x82);
    st25r3916_change_reg_bits(handle, ST25R3916_REG_ANT_TUNE_B, 0xff, 0x82);
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_OP_CONTROL,
        ST25R3916_REG_OP_CONTROL_en_fd_mask,
        ST25R3916_REG_OP_CONTROL_en_fd_auto_efd);

    // Perform calibration
    if(st25r3916_check_reg(
           handle, ST25R3916_REG_REGULATOR_CONTROL, ST25R3916_REG_REGULATOR_CONTROL_reg_s, 0x00)) {
        //FURI_LOG_I(TAG, "Adjusting regulators");
        // Reset logic
        st25r3916_set_reg_bits(
            handle, ST25R3916_REG_REGULATOR_CONTROL, ST25R3916_REG_REGULATOR_CONTROL_reg_s);
        st25r3916_clear_reg_bits(
            handle, ST25R3916_REG_REGULATOR_CONTROL, ST25R3916_REG_REGULATOR_CONTROL_reg_s);
        st25r3916_direct_cmd(handle, ST25R3916_CMD_ADJUST_REGULATORS);
        furi_delay_ms(6);
    }

    furi_hal_nfc_low_power_mode_start();

    furi_hal_nfc_release();
}

int32_t mifare_nested_app(void* p) {
    UNUSED(p);
    // Do required stuff
    remove_new_nfc_and_init_old();

    MifareNested* mifare_nested = mifare_nested_alloc();

    scene_manager_next_scene(mifare_nested->scene_manager, MifareNestedSceneStart);

    view_dispatcher_run(mifare_nested->view_dispatcher);

    mifare_nested_free(mifare_nested);

    // Lets hope nothing goes wrong
    remove_old_nfc_and_init_new_back();

    return 0;
}
