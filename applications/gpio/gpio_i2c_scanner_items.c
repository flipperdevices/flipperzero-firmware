#include "gpio_i2c_scanner_items.h"
#include <furi_hal_delay.h>


void gpio_i2c_scanner_run_once(I2CScannerState* i2c_scanner_state){
    i2c_scanner_state->items = 0;
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);

    uint32_t response_timeout_ticks = furi_hal_ms_to_ticks(5.f);

    //Addresses 0 to 7 are reserved and won't be scanned
    for(int i = 8; i<=127; i++){
        if(furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, i<<1, response_timeout_ticks)){//Bitshift of 1 bit to convert 7-Bit Address into 8-Bit Address
            i2c_scanner_state->responding_address[i2c_scanner_state->items] = i;
            i2c_scanner_state->items++;
        }
    }
    
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
}
