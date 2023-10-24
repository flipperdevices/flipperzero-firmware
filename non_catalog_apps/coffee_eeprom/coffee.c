#include "coffee.h"
#include "math.h"
#include <furi.h>
#include <stdio.h>

#define EEPROM_I2C_ADDR (0x50 << 1)

uint8_t data_buffer[4];
uint8_t address_54_buffer[4];
uint8_t address_44_buffer[4];

bool write_buffer(uint8_t *buffer, size_t buffer_size, uint8_t start_address){
	furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
	bool result = false;
	if(furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, EEPROM_I2C_ADDR, (uint32_t) 1000)){
		FURI_LOG_E("COFFEE", "WRITE READY");
		for (size_t i = 0; i < buffer_size; i++){	
			result = false;
			while(!result){
				result = furi_hal_i2c_write_reg_8(&furi_hal_i2c_handle_external, EEPROM_I2C_ADDR, start_address + i, buffer[i], (uint32_t) 2000);
            	FURI_LOG_E("COFFEE", "Write %.2X, byte %d/%d at address %.2X, result %d", buffer[i], i + 1, buffer_size, start_address + i, result);
			}
		}
	}
	else{
        FURI_LOG_D("COFFEE", "VIRGIN: EEPROM not ready %x (8-bit)", EEPROM_I2C_ADDR);
    }
	furi_hal_i2c_release(&furi_hal_i2c_handle_external);
	return result;
}
void write_dump(uint8_t* buffer, size_t size){    
	write_buffer(buffer, size, 0x00);
}

void write_credit(float value){
	calc_credit(value, address_44_buffer);
	memcpy(address_54_buffer, address_44_buffer, 4 * sizeof(uint8_t));
    address_54_buffer[1] -= 0x40;
	write_buffer(address_44_buffer, sizeof(address_44_buffer), 0x44);
	write_buffer(address_54_buffer, sizeof(address_54_buffer), 0x54);
}

void dump(uint8_t* out){
	FuriString* dump_str = furi_string_alloc();
	furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
	if(furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, EEPROM_I2C_ADDR, (uint32_t) 1000)){
		uint8_t temp[1];
		FURI_LOG_E("COFFEE_eeprom", "Start dump");
		for (size_t i=0; i<256; i++){
			furi_hal_i2c_read_reg_8(&furi_hal_i2c_handle_external, EEPROM_I2C_ADDR, 0x00 + i, (uint8_t *) temp, (uint32_t) 500);
			furi_string_cat_printf(dump_str, "%.2X", temp[0]);
			out[i] = temp[0];
		}
		FURI_LOG_E("COFFEE_eeprom", furi_string_get_cstr(dump_str));
		FURI_LOG_E("COFFEE_eeprom", "End dump");
	}else{
        FURI_LOG_D("COFFEE", "DUMP: EEPROM not ready %x (8-bit)", EEPROM_I2C_ADDR);
    }
	furi_hal_i2c_release(&furi_hal_i2c_handle_external);
}

float read_credit(){
    memset(data_buffer, 0, sizeof(data_buffer)); //reset array
	furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
	bool is_ready = false;
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
		return credit / 100.00;
	}
	else{
		furi_hal_i2c_release(&furi_hal_i2c_handle_external);
		FURI_LOG_D("COFFEE", "READ CREDIT: EEPROM not ready %x (8-bit)", EEPROM_I2C_ADDR);
		return -1.0;
	}
}
void calc_credit(float value, uint8_t* result){
	//credit
	uint8_t coeff[8];
	uint16_t n = value * 100;
    for (size_t i = 0; i < 8; i++) {
        coeff[i] = n % 4;
        n /= 4;
    }
	uint8_t credit[4];
	for (size_t i = 0; i < 4; i++) {
		uint8_t bit3 = (coeff[7 - i] >> 1) & 1;
		uint8_t bit2 = coeff[7 - i] & 1;
		uint8_t bit1 = (coeff[3 - i] >> 1) & 1;
		uint8_t bit0 = coeff[3 - i] & 1;
		credit[i] = (bit3 << 3) | (bit2 << 2) | (bit1 << 1) | bit0;
    }
	//checksum
	uint8_t exponents[] = {12, 8, 4, 0};
	uint16_t numerator = value * 100;
	uint8_t sub_factor = 0;

	for (size_t i = 0; i < 4; i++) {
		sub_factor += numerator / (1 << exponents[i]);
		numerator = numerator % (1 << exponents[i]);
	}
	
	uint8_t subbed = 187 - sub_factor;
	
	uint8_t checksum[4];
	for (size_t i = 0; i < 4; i++) {
        checksum[3 - i] = (subbed % 4) * 4;
        subbed /= 4;
    }
    //concatnate
    for (size_t i = 0; i < 4; i++) {
        result[i] = (checksum[i] << 4) | credit[i];
    }
}