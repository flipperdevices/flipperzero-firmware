#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_main_menu.h"
#include <furi_hal_cortex.h>

float hc_sr04_ms_to_m(uint32_t ms) {
    const float speed_sound_m_per_s = 343.0f;
    const float time_s = ms / 1e3f;
    const float total_dist = time_s * speed_sound_m_per_s;
    return total_dist / 2.0f;
}

float hc_sr04_cycles_to_m(uint32_t cycles) {
    const float speed_sound_m_per_s = 343.0f;
    const float time_s = cycles / furi_hal_cortex_instructions_per_microsecond() / 1000 / 1e3f;
    const float total_dist = time_s * speed_sound_m_per_s;
    return total_dist / 2.0f;
}

void hcsr04_dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);
    App* app = context;
    UNUSED(app);
    if(result == DialogExResultLeft) {
        FURI_LOG_I(TAG, "DialogExResultLeft ");
    } else if(result == DialogExResultRight) {
        FURI_LOG_I(TAG, "DialogExResultRight");
        //furi_hal_gpio_init_simple(&gpio_ext_pc1, GpioModeOutputPushPull);
        furi_hal_gpio_init(&gpio_ext_pc1, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
        furi_hal_gpio_init(&gpio_ext_pc0, GpioModeInput, GpioPullNo, GpioSpeedVeryHigh);
        furi_hal_gpio_write(&gpio_ext_pc1, true);
        furi_delay_ms(10);
        furi_hal_gpio_write(&gpio_ext_pc1, false);

        furi_hal_gpio_init_simple(&gpio_ext_pc0, GpioModeInput);
        bool v = false;
        
        bool valid = true;
        size_t beginning = DWT->CYCCNT;
        while(v != true)
        {
            v = furi_hal_gpio_read(&gpio_ext_pc0);
            if(DWT->CYCCNT - beginning > 1000000)
            {
                FURI_LOG_I(TAG, "Pulse did not start");
                valid = false;
                break;
            }
        }
        size_t p_us_start = DWT->CYCCNT;
        uint32_t p_ms_start = furi_get_tick();

        while(v != false)
        {
            v = furi_hal_gpio_read(&gpio_ext_pc0);
            if(DWT->CYCCNT - beginning > 1000000)
            {
                FURI_LOG_I(TAG, "Pulse did not end");
                valid = false;
                break;
            }
        }
        size_t p_us_end = DWT->CYCCNT;
        uint32_t p_ms_end = furi_get_tick();
        FURI_LOG_I(TAG, "Duration: %ld ms", p_ms_end - p_ms_start);
        FURI_LOG_I(TAG, "Duration: %d us", p_us_end - p_us_start);

        char str[22];
        if(valid){
            float res = hc_sr04_ms_to_m(p_ms_end - p_ms_start);
            snprintf(app->state->value, 22, "%02ffm", (double)res);}
        else
            snprintf(app->state->value, 22, "invalid: %ld", p_ms_end - p_ms_start);
        
        snprintf(app->state->value, 22, "%02fms %02fus",
            (double)hc_sr04_ms_to_m(p_ms_end - p_ms_start),
            (double)hc_sr04_cycles_to_m(p_us_end - p_us_start));


        FURI_LOG_I(TAG, "Value: %s", str);
        dialog_ex_set_header(app->dialog, app->state->value, 64, 12, AlignCenter, AlignTop);
        //dialog_ex_set_text(app->dialog, str, 64, 25, AlignCenter, AlignTop);
    }
}

void fcom_hcsr04_scene_on_enter(void* context) {
    FURI_LOG_I(TAG, "fcom_hcsr04_scene_on_enter");
    UNUSED(context);
    App* app = context;

    dialog_ex_set_header(app->dialog, "HC-SR04", 64, 12, AlignCenter, AlignTop);
    dialog_ex_set_left_button_text(app->dialog, "Exit");
    dialog_ex_set_right_button_text(app->dialog, "Measure");
    dialog_ex_set_result_callback(app->dialog, hcsr04_dialog_callback);
    dialog_ex_set_context(app->dialog, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, FcomHCSR04View);
}

bool fcom_hcsr04_scene_on_event(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "fcom_hcsr04_scene_on_event");
    UNUSED(context);
    UNUSED(event);

    return false; //consumed event
}

void fcom_hcsr04_scene_on_exit(void* context) {
    FURI_LOG_I(TAG, "fcom_hcsr04_scene_on_exit");
    UNUSED(context);
    App* app = context;
    dialog_ex_reset(app->dialog);
}


