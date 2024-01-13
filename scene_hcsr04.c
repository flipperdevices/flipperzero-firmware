#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_main_menu.h"
#include <furi_hal_cortex.h>


double hc_sr04_cycles_to_m(uint32_t cycles) {
    const double speed_sound_m_per_s = 343.0f;
    double ipus = furi_hal_cortex_instructions_per_microsecond();
    // Run multiplication first for accuracy
    const double total_dist = cycles * speed_sound_m_per_s / ipus / 1000.0l / 1000.0l;
    return total_dist / 2.0l;
}

typedef struct Context {
    uint32_t count;
    uint32_t rise;
    uint32_t fall;
    FuriSemaphore* lock;
} Context;

static void pulse_ISR(void* ctx) {
    Context* context = (Context *)ctx;
    context->count += 1;
    if(furi_hal_gpio_read(&gpio_ext_pc0)) {
        context->rise = DWT->CYCCNT;//furi_get_tick();
        furi_hal_gpio_write(&gpio_ext_pc3, true);
    } else {
        context->fall = DWT->CYCCNT;//furi_get_tick();
        furi_hal_gpio_write(&gpio_ext_pc3, false);
        furi_semaphore_release(context->lock);
    }
}


void hcsr04_dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);
    App* app = context;
    UNUSED(app);
    if(result == DialogExResultLeft) {
        FURI_LOG_I(TAG, "DialogExResultLeft ");
    } else if(result == DialogExResultRight) {
        FURI_LOG_I(TAG, "DialogExResultRight");
        FuriSemaphore* s = furi_semaphore_alloc(1, 0);
        Context ctx;
        ctx.rise = 0;
        ctx.fall = 0;
        ctx.count = 0;
        ctx.lock = s;
        // Setup our pulse to trigger pin
        furi_hal_gpio_init(&gpio_ext_pc1, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
        
        // Setup our debug output testing pin
        furi_hal_gpio_init(&gpio_ext_pc3, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
        
        // Setup our pulse read pin
        furi_hal_gpio_init(&gpio_ext_pc0, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedVeryHigh);
        furi_hal_gpio_remove_int_callback(&gpio_ext_pc0);
        furi_hal_gpio_add_int_callback(
            &gpio_ext_pc0,
            pulse_ISR,
            (void*)&ctx);
        
        // Pulse to trigger read
        furi_hal_gpio_write(&gpio_ext_pc1, true);
        furi_delay_ms(10);
        furi_hal_gpio_write(&gpio_ext_pc1, false);

        FuriStatus status = furi_semaphore_acquire(s, 1000);
        furi_semaphore_free(s);
        furi_hal_gpio_remove_int_callback(&gpio_ext_pc0);
        bool valid = status == FuriStatusOk;

        char str[22];
        if(valid) {
            snprintf(app->state->value, 22, "%02fm %ldc %ldn",
                //(double)hc_sr04_ms_to_m(p_ms_end - p_ms_start),
                hc_sr04_cycles_to_m(ctx.fall - ctx.rise),
                ctx.fall - ctx.rise,
                ctx.count);
        }
        else
        {
            snprintf(app->state->value, 22, "%d %ld %ld", status, ctx.rise, ctx.fall);
        }

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


