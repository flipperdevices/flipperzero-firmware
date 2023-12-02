/**
 * @file TEA5767.c
 * @author Coolshrimp - CoolshrimpModz.com
 * @brief Library for controlling the TEA5767 FM radio chip.
 * @version 0.1
 * @date 2023-09-29
 * 
 * @copyright GPLv3
 */
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>
#include <stdio.h> // Include necessary libraries

#include "TEA5767.h"

#define TIMEOUT_MS 100
// Define a structure to store station information

struct StationInfo {
    float frequency; // Frequency in MHz
    int signalLevel; // Signal level
};

// Helper function to acquire I2C
static bool acquire_i2c() {
    // Acquire I2C and check for device readiness
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    return furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, TEA5767_ADR, 5);
}

static void release_i2c() {
    // Release I2C
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
}

bool tea5767_is_device_ready() {
    bool result = acquire_i2c();
    release_i2c();
    return result;
}

bool tea5767_read_registers(uint8_t* buffer) {
    if (buffer == NULL) return false;
    bool result = acquire_i2c();
    if(result) {
        result = furi_hal_i2c_rx(&furi_hal_i2c_handle_external, TEA5767_ADR, buffer, 5, TIMEOUT_MS);
    }
    release_i2c();
    return result;
}

bool tea5767_write_registers(uint8_t* buffer) {
    bool result = false;
    if (buffer == NULL) return false;  // Added NULL check
        furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
        result = furi_hal_i2c_tx(&furi_hal_i2c_handle_external, TEA5767_ADR, buffer, 5, TIMEOUT_MS);
        furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return result;
}

bool tea5767_init(uint8_t* buffer) {
    bool result = false;

    buffer[0] = 0x00;
    buffer[1] = 0x00;
    buffer[2] = 0xB0;
    buffer[3] = REG_4_XTAL | REG_4_SMUTE;
    buffer[4] = 0x00;

    result = tea5767_write_registers(buffer);
    return result;
}

bool tea5767_set_mute(uint8_t* buffer, bool mute) {
    bool result = false;

    if(mute) {
        buffer[REG_1] |= REG_1_MUTE;
    } else {
        buffer[REG_1] &= ~REG_1_MUTE;
    }

    result = tea5767_write_registers(buffer);
    return result;
}

bool tea5767_set_stereo(uint8_t* buffer, bool stereo) {
    bool result = false;

    if(stereo) {
        buffer[REG_3] &= ~REG_3_MS;
    } else {
        buffer[REG_3] |= REG_3_MS;
    }

    result = tea5767_write_registers(buffer);
    return result;
}

bool tea5767_seek(uint8_t* buffer, bool seek_up) {
    bool result = false;
    if (buffer == NULL) {return false;} 
    buffer[REG_1] |= REG_1_SM;    // Set the Search Mode (SM) bit to initiate seek
    if(seek_up) {    
        buffer[REG_3] |= REG_3_SUD;  // Set Search Up (SUD) bit
    } else {
        buffer[REG_3] &= ~REG_3_SUD;  // Set Search Down (SUD) bit 
    }
    buffer[REG_3] |= (REG_3_SSL | 0x60); // Set the Search Stop Level (SSL) to high for better tuning accuracy,  set bit 7 for RSSI 7
    buffer[REG_3] &= ~REG_3_MS;  // Set stereo mode (clearing the Mono bit)   
    buffer[REG_4] &= ~REG_4_BL;   // Limit FM band 87.5 - 108 MHz.        
    buffer[REG_5] |= REG_5_PLLREF;
    // Write the updated register values to the TEA5767
    result = tea5767_write_registers(buffer);
    return result;    
}

bool tea5767_get_frequency(uint8_t* buffer, int* value) {
    bool result = false;
    uint16_t frequency;
    if (buffer == NULL || value == NULL) return false;  //NULL check
    if (tea5767_read_registers(buffer)) {
        frequency = ((buffer[REG_1] & REG_1_PLL) << 8) | buffer[1];
        *value = (frequency * QUARTZ / 4 - FILTER) / 10000;
        result = true;
    }
    return result;
}

bool tea5767_set_frequency(uint8_t* buffer, int value) {
    bool result = false;
    if (buffer == NULL) {return false;}
    uint16_t frequency = 4 * (value * 10000 + FILTER) / QUARTZ;
    buffer[REG_1] = ((buffer[0] & ~REG_1_PLL) | ((frequency >> 8) & REG_1_PLL));  // Set the upper 8 bits of the PLL word
    buffer[REG_2] = frequency & REG_2_PLL;  // Set the lower 8 bits of the PLL word
    buffer[REG_3] &= ~REG_3_MS;  // Set stereo mode (clearing the Mono bit)   
    buffer[REG_5] |= REG_5_PLLREF;
    result = tea5767_write_registers(buffer);
    return result;
}

bool tea5767_get_radio_info(uint8_t* buffer, struct RADIO_INFO* info) {
    bool result = false;
    int frequency_khz; 

    // Error handling: Check if buffer and info are not NULL
    if (buffer && info && tea5767_read_registers(buffer)) {
        
        if (buffer[REG_3] & REG_3_MS) {
            info->stereo = true;
        } else {
            info->stereo = false;
        }

        info->signalLevel = buffer[REG_4] >> 4;

        // Determine signal quality based on signal level
        if (info->signalLevel >= 0 && info->signalLevel <= 3) {
            strncpy(info->signalQuality, "Poor", sizeof(info->signalQuality));
        } else if (info->signalLevel >= 4 && info->signalLevel <= 7) {
            strncpy(info->signalQuality, "Fair", sizeof(info->signalQuality));
        } else if (info->signalLevel >= 8 && info->signalLevel <= 11) {
            strncpy(info->signalQuality, "Good", sizeof(info->signalQuality));
        } else if (info->signalLevel >= 12 && info->signalLevel <= 15) {
            strncpy(info->signalQuality, "Excellent", sizeof(info->signalQuality));
        } else {
            strncpy(info->signalQuality, "Unknown", sizeof(info->signalQuality));
        }

        // Now get the frequency
        if (tea5767_get_frequency(buffer, &frequency_khz)) {
            info->frequency = frequency_khz / 100.0f;  // Convert kHz to MHz
            result = true;  // Only return true if both read_registers and get_frequency succeeded
        }

         // Check if the radio is muted
        if (buffer[REG_1] & REG_1_MUTE) {
            info->muted = true;
        } else {
            info->muted = false;
        }

    }
    return result;
}

void tea5767_seekUp() {    
    //Get CUrrent Station
    double fm_frequency = tea5767_GetFreq();
    int targetFrequencyKHz = fm_frequency * 100;
        
    uint8_t buffer[5];
    if(tea5767_init(buffer)) { 
        tea5767_set_frequency(buffer, targetFrequencyKHz);
        // Start seeking upwards
        tea5767_seek(buffer, true);
    }
}

void tea5767_seekDown() {
        //Get CUrrent Station
    double fm_frequency = tea5767_GetFreq();
    int targetFrequencyKHz = fm_frequency * 100;     
    
    uint8_t buffer[5];
    if(tea5767_init(buffer)) {   
        tea5767_set_frequency(buffer, targetFrequencyKHz);
        // Start seeking upwards
        tea5767_seek(buffer, false);
    }
}

void tea5767_ToggleMute() {
    uint8_t buffer[5];
    if(tea5767_read_registers(buffer)) {
        if((buffer[REG_1] & REG_1_MUTE) == 0) {
            tea5767_set_mute(buffer, true);
        } else {
            tea5767_set_mute(buffer, false);
        }
    }
}

void tea5767_MuteOn() {
    uint8_t buffer[5];
    if(tea5767_read_registers(buffer)) {  // Read the current state into the buffer
        tea5767_set_mute(buffer, true);  // Set the mute bit
    }
}

void tea5767_MuteOff() {
    uint16_t frequency;
    float value;  // Changed to a float variable, not a pointer
    uint8_t buffer[5];
    if (tea5767_read_registers(buffer)) {  // Read the current state into the buffer
        tea5767_set_mute(buffer, false);  // Clear the mute bit
        frequency = ((buffer[0] & REG_1_PLL) << 8) | buffer[1];
        value = (float)(frequency * QUARTZ / 4 - FILTER) / 10000;  // Explicitly cast to float
        tea5767_SetFreqMHz(value/ 100.0);  // Pass the float value, not the pointer
    }
}

void tea5767_SetFreqKHz(int freq_khz) { 
    uint8_t buffer[5];
    if (tea5767_init(buffer)) {
        tea5767_set_frequency(buffer, freq_khz);
    }
}

void tea5767_SetFreqMHz(float freq_mhz) { 
    uint8_t buffer[5];
    if (tea5767_init(buffer)) {
        int freq_khz = (int)(freq_mhz * 100.0);  // Convert MHz to kHz
        tea5767_set_frequency(buffer, freq_khz);
    }
}

float tea5767_GetFreq() {
    uint8_t buffer[5];
    int value;
    if(tea5767_get_frequency(buffer, &value)) {
        return value / 100.0;  // Convert to MHz
    }
    return -1;  // Error
}

void tea5767_sleep(uint8_t *buffer) {
    if(tea5767_read_registers(buffer)) {        
        buffer[REG_4] |= REG_4_STBY; // Set the Standby bit in register 4 to enter standby mode
        tea5767_write_registers(buffer);
    }
}
