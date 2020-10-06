# GPIO

GPIO defined as struct `GpioPin`.

GPIO functions:

```C
// Init GPIO
void gpio_init(GpioPin* gpio, GpioMode mode);

typedef enum { GpioModeInput, GpioModeOutput, GpioModeOpenDrain } GpioMode;

// write value to GPIO
void gpio_write(GpioPin* gpio, bool state);

// read value from GPIO, f = LOW, t = HIGH
bool gpio_read(GpioPin* gpio);
```

When application is exited, system place pin to Z-state by calling `gpio_disable`.

```C
// put GPIO to Z-state (used for restore pin state on app exit)
void gpio_disable(ValueMutex* gpio_mutex) {
    GpioPin* gpio = acquire_mutex(gpio_mutex, 0);
    gpio_init(gpio, GpioModeInput);
    release_mutex(gpio_mutex, gpio);
}
```

Available GPIO stored in FURI as `ValueMutex<GpioPin*>`.

```C
inline static ValueMutex* open_gpio_mutex(const char* name) {
    ValueMutex* gpio_mutex = (ValueMutex*)furi_open(name);
    if(gpio_mutex != NULL) flapp_on_exit(gpio_disable, gpio_mutex);

    return gpio_mutex;
}

// helper
inline static GpioPin* open_gpio(const char* name) {
    ValueMutex* gpio_mutex = open_gpio(name);
    return acquire_mutex(gpio_mutex, 0);
}
```

## Available GPIO (F2)

* PA4
* PA5
* PA6
* PA7
* PB2
* PC3
* PC0
* PC1
* PB6
* PB7
* PA13
* PA14
* RFID_PULL
* IR_TX
* IBUTTON

## Usage example

```C
void gpio_example() {
    GpioPin* pin = open_gpio("PB6");

    if(pin == NULL) {
        printf("pin not available\n");
        return;
    }
    
    gpio_init(pin, GpioModeOutput);

    while(1) {
        gpio_write(pin, true);
        delay(100);
        gpio_write(pin, false);
        delay(100);
    }
}
```

# PWM

## Available PWM (F2)

* SPEAKER
* RFID_OUT


# ADC
