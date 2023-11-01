# GPIO_BLINK_PWM
This is the classic "Hello World" of GPIO, where we blink an LED using PWM (Pulse Width Modulation).  You will need an LED (any color is fine) and a resistor (220 ohms - 1 kilohm should be fine).  Connect the long side of the LED into pin A7.  Connect the short size of the LED to one side of the resistor (doesn't matter which side).  Connect the other side of the resistor to any of the pin GND.

Run the application and the LED should start blinking.  Hold the back button to exit.

## How it works
- Pulse width modulation is supported on pin A7 using FuriHalPwmOutputIdTim1PA7.  You can also do pulse width modulation on pin A4 using FuriHalPwmOutputIdLptim2PA4.  Using PWM has the advantage that the voltage on the pins transition automatically at some fixed frequency and duty cycle, without your code having to keep changing the state of the pins.  The PWM library only supports an integer for the frequency, so it will not work for if you need to blink slower than 1 Hz (1 time per second).  The duty cycle is the percentage of time the light should be on (so it should typically be a number between 1 and 99).

- We define a variable for our LED...
  ```c
  const FuriHalPwmOutputId channel_led = FuriHalPwmOutputIdTim1PA7;
  ```

- We start the PWM timer, blinking LED at 1 Hz, 20% duty cycle (the pin will be at 3.3 volts 20% of the time, then 0 volts the remainder).
  ```c
  
  furi_hal_pwm_start(channel_led, 1, 20);
  ```

- In a loop we sleep if the back button is not currently pressed.
  ```c
  while(furi_hal_gpio_read(pin_back)) {
    furi_delay_ms(100);
  }
  ```

- We stop the PWM timer.  This stops the timer and sets the pin to analog (floating input).
  ```c
  
  furi_hal_pwm_stop(channel_led);
  ```