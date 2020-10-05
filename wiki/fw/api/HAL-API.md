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

# SPI

HAL struct `SPI_HandleTypeDef*` used for handling SPI info.

For transmit/receive data use `spi_xfer` function:

```C
bool spi_xfer(
    SPI_HandleTypeDef* spi,
    uint8_t* tx_data, uint8_t* rx_data, size_t len,
    PubSubCallback cb, void* ctx);
```

* `tx_data` and `rx_data` size must be equal (and equal `len`)
* `cb` called after spi operation is completed, `(NULL, ctx)` passed to callback.

Blocking verison:

```C
inline static bool spi_xfer_block(SPI_HandleTypeDef* spi, uint8_t* tx_data, uint8_t* rx_data, size_t len) {
    semaphoreInfo s;
    osSemaphore block = createSemaphoreStatic(s);
    if(!spi_xfer(spi, tx_data, rx_data, len, RELEASE_SEMAPHORE, (void*)block)) {
        osReleaseSemaphore(block);
        return false;
    }
    osWaitSemaphore(block);
    return false;
}
```

# System devices

API available as `ValueMutex<Cc1101Bus>`:

## CC1101

```C
typedef struct {
    GpioPin* cs; ///< CS pin
    ValueMutex* spi; ///< <SPI_HandleTypeDef*>
    PubSub* irq;
} Cc1101Bus;
```

You can get API instance by calling `open_cc1101_bus`:

```C
/// Get input struct
inline Cc1101Bus* open_cc1101_bus(const char* name) {
    return (Cc1101Bus*)furi_open(name);
}
```

Use `subscribe_pubsub` for subscribinq to irq events.

Default bus name is `/dev/cc1101_bus`.

### Application example

```C
// Be careful, this function called from IRQ context
void handle_irq(void* _arg, void* _ctx) {
}

void cc1101_example() {
    ValueMutex* cc1101_bus_mutex = open_input("/dev/cc1101_bus");
    if(cc1101_bus_mutex == NULL) return; // bus not available, critical error

    {
        // acquire bus 
        Cc1101Bus* cc1101_bus = acquire_mutex_block(cc1101_bus_mutex);

        // subscribe to IRQ
        PubSubId* irq_subscriber = subscribe_pubsub(cc1101_bus->irq, handle_irq, NULL);

        // make transaction
        uint8_t request[4] = {0xDE, 0xAD, 0xBE, 0xEF};
        uint8_t response[4];

        {
            SPI_HandleTypeDef* spi = acquire_mutex_block(cc1101_bus->spi);

            gpio_write(cc1101_bus->cs, false);
            spi_xfer_block(spi, request, response, 4);
            gpio_write(cc1101_bus->cs, true);

            release_mutex(cc1101_bus->spi, spi);
        }

        // release bus
        release_mutex(cc1101_bus_mutex, cc1101_bus);
    }
}
```