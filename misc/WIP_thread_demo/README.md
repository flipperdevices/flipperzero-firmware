# WIP thread demo

The Flipper Zero supports the concepts of threads. The idea is that while your main application is running some code, another thread can be running different code in the same application process, at the same time. The reality is likely that the processor will end up switching back and forth between the various threads. Threads can get paused momentarily, while processing instructions from another thread. A thread can set its priority by calling `furi_thread_set_current_priority(FuriThreadPriorityHigh)`, passing the new priority of the thread. Warning -- shanging the priority could cause other threads to not get time to execute.

In this demonstration, we use a thread to blink pin A7 on/off quickly, effectively creating a PWM signal and varying the brightness of an LED connected to pin A7. Our main thread sets information, such as the frequency and duty cycle (percentage of time to have the pin on.)

A thread is allocated using a command like `FuriThread* thread = furi_thread_alloc_ex("MyThread", 2048, my_thread_callback, context);`. The first parameter is the friendly name for the thread. The second parameter is the stack size. The third parameter is the method that the thread should run, with a signature like `int32_t my_thread_callback(void* context)`. The fourth parameter is the context to pass to the method.

After you have allocated the thread, you can call `furi_thread_flags_set` passing the `furi_thread_get_id` of the Thread and the bits you want to set. When the main application is done, it should signal to the thread that it should terminate, and then the main application should do a `furi_thread_join` to wait for the thread to complete.

In the thread callback, it can get the flags, clear the flags, and wait on flags. Typically you want your thread to check periodically to see if it should be exiting (for example, because the main application is exiting.)

NOTE: This current demo has a flicker when the PWM is a low percentage, because while the code in the thread checking to see if it should exit is running, the LED is not blinking. This is just a demo app of how to use a Thread. If you really want to do PWM, you should look into the `furi_hal_pwm_start` API, or the underlying LL*TIM* functions in `lib\stm32wb_hal\Inc\stm32wbxx_ll_tim.h`.
