#include <furi.h>
#include <furi_hal.h>

#define TAG "WIP_Thread_Demo"

const GpioPin* pin = &gpio_ext_pa7;

typedef struct {
    uint8_t duty_cycle;
    uint32_t freq;
} MyThreadContext;

typedef enum {
    MyThreadFlagUpdated = 1 << 0,
    MyThreadFlagFoo = 1 << 1,
    MyThreadFlagExiting = 1 << 2,
    MyThreadFlagAll = (1 << 3) - 1,
} MyThreadFlags;

int32_t my_thread_callback(void* context) {
    MyThreadContext* mtc = (MyThreadContext*)context;
    furi_thread_flags_clear(MyThreadFlagAll);

    uint32_t cycle = 1000000 / mtc->freq;
    uint32_t cycle_on = cycle * mtc->duty_cycle / 100;
    uint32_t cycle_off = cycle - cycle_on;
    uint32_t num_cycles = mtc->freq;

    MyThreadFlags flags;
    while(((flags = furi_thread_flags_get()) & MyThreadFlagExiting) != MyThreadFlagExiting) {
        // Do some work (for ~1 second) then check to see if thead should exit...

        // NOTE: This causes some "flicker" because the LED is not changing states
        // while we do the above and below checks.

        if(flags & MyThreadFlagUpdated) {
            cycle = 1000000 / mtc->freq;
            cycle_on = cycle * mtc->duty_cycle / 100;
            cycle_off = cycle - cycle_on;
            num_cycles = mtc->freq;
            furi_thread_flags_clear(MyThreadFlagUpdated);
        }

        for(uint32_t i = 0; i < num_cycles * 3; i++) {
            // Turn on
            furi_hal_gpio_write(pin, true);
            furi_delay_us(cycle_on);
            // Turn off
            furi_hal_gpio_write(pin, false);
            furi_delay_us(cycle_off);
        }
    };

    FURI_LOG_D(TAG, "Exiting the thread");

    return 0;
}

int thread_demo_app(char* p) {
    UNUSED(p);

    furi_hal_gpio_init_simple(pin, GpioModeOutputPushPull);

    MyThreadContext* context = (MyThreadContext*)malloc(sizeof(MyThreadContext));
    context->duty_cycle = 100;
    context->freq = 30000;

    FuriThread* thread = furi_thread_alloc_ex("MyThread", 2048, my_thread_callback, context);
    furi_thread_start(thread);

    // This would normally do some work.  We sleep here just as a demo..
    FURI_LOG_D(TAG, "Running the main program");
    furi_delay_ms(3000);

    context->duty_cycle = 50;
    furi_thread_flags_set(furi_thread_get_id(thread), MyThreadFlagUpdated);
    FURI_LOG_D(TAG, "Set duty cycle to %d.", context->duty_cycle);
    furi_delay_ms(3000);

    context->duty_cycle = 10;
    furi_thread_flags_set(furi_thread_get_id(thread), MyThreadFlagUpdated);
    FURI_LOG_D(TAG, "Set duty cycle to %d.", context->duty_cycle);
    furi_delay_ms(3000);

    context->duty_cycle = 2;
    furi_thread_flags_set(furi_thread_get_id(thread), MyThreadFlagUpdated);
    FURI_LOG_D(TAG, "Set duty cycle to %d.", context->duty_cycle);
    furi_delay_ms(3000);
    FURI_LOG_D(TAG, "Exiting the main program");

    // Our app is done.  Release any resources...

    // Tell the thread we are exiting.
    furi_thread_flags_set(furi_thread_get_id(thread), MyThreadFlagExiting);
    // Wait for thread to exit.
    furi_thread_join(thread);

    furi_hal_gpio_init_simple(pin, GpioModeAnalog);

    return 0;
}