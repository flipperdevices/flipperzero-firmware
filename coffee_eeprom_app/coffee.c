#include "coffee.h"
#include "math.h"
#include <furi.h>
#include <stdio.h>

#define EEPROM_I2C_ADDR (0x50 << 1)

uint8_t data_buffer[4];
uint8_t address_54_buffer[4] = { 0x83, 0x42, 0x02, 0x8C };
uint8_t address_44_buffer[4] = { 0x83, 0x82, 0x02, 0x8C };

uint8_t virgin_buffer[128] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

bool write_buffer(uint8_t *buffer, size_t buffer_size, uint8_t start_address){
	furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
	bool result = false;
	if(furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, EEPROM_I2C_ADDR, (uint32_t) 1000)){
		for (size_t i = 0; i < buffer_size; i++){	
			result = false;
			while(!result){
				result = furi_hal_i2c_write_reg_8(&furi_hal_i2c_handle_external, EEPROM_I2C_ADDR, start_address + i, buffer[i], (uint32_t) 2000);
            	FURI_LOG_D("COFFEE", "Write %.2X, byte %d/%d at address %.2X, result %d", buffer[i], i + 1, buffer_size, start_address + i, result);
			}
		}
	}
	else{
        FURI_LOG_D("COFFEE", "VIRGIN: EEPROM not ready %x (8-bit)", EEPROM_I2C_ADDR);
    }
	furi_hal_i2c_release(&furi_hal_i2c_handle_external);
	return result;
}
void virgin(){     
	write_buffer(virgin_buffer, sizeof(virgin_buffer), 0x00);
}

void write_10_eur(){
	write_buffer(address_44_buffer, sizeof(address_44_buffer), 0x44);
	write_buffer(address_54_buffer, sizeof(address_54_buffer), 0x54);
}

void dump(){
	furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
	if(furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, EEPROM_I2C_ADDR, (uint32_t) 1000)){
		uint8_t temp[1];
		char str[513];
		int index = 0;
		FURI_LOG_E("COFFEE_eeprom", "Start dump");
		for (size_t i=0; i<256; i++){
			furi_hal_i2c_read_reg_8(&furi_hal_i2c_handle_external, EEPROM_I2C_ADDR, 0x00 + i, (uint8_t *) temp, (uint32_t) 500);
			index += snprintf(&str[index], sizeof(str)-index, "%.2X", temp[0]);
		}
		FURI_LOG_E("COFFEE_eeprom", str);
		FURI_LOG_E("COFFEE_eeprom", "End dump");
	}else{
        FURI_LOG_D("COFFEE", "DUMP: EEPROM not ready %x (8-bit)", EEPROM_I2C_ADDR);
    }
	furi_hal_i2c_release(&furi_hal_i2c_handle_external);
}

double read_credit(){
    memset(data_buffer, 0, sizeof(data_buffer)); //reset array
	furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
	bool is_ready = false;
	while(!is_ready){
		is_ready = furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, EEPROM_I2C_ADDR, (uint32_t) 1000);
		if(is_ready){
			furi_hal_i2c_read_mem(&furi_hal_i2c_handle_external, EEPROM_I2C_ADDR, 0x44, data_buffer, sizeof(data_buffer), (uint32_t) 1000);
			int credit = 0;
			int exponent = 14;
			int hi, lo = 0;
			for (size_t i = 0; i < sizeof(data_buffer); i++)
			{  //iterate 2 bit at times
				hi = 0;
				lo = 0;
				for (int j = 3; j >= 0; j--) {
					int k = (data_buffer[i] % 16) >> j; // right shift
					if (k & 1){
						if(j>=2)
							hi += pow(2, j-2);
						else
							lo += pow(2, j);
					}
				}
				credit += hi * pow(2, exponent) + lo * pow(2, exponent-8);
				exponent -= 2;
			}
			furi_hal_i2c_release(&furi_hal_i2c_handle_external);
			return credit / 100.0;
		}
		else{
			FURI_LOG_D("COFFEE", "READ CREDIT: EEPROM not ready %x (8-bit)", EEPROM_I2C_ADDR);
		}
	}
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
	return 0.0;
}