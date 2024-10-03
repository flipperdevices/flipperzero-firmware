#include "bq27220.h"
#include "bq27220_reg.h"
#include "bq27220_data_memory.h"

_Static_assert(sizeof(BQ27220DMGaugingConfig) == 2, "Incorrect structure size");

#include <furi.h>
#include <stdbool.h>

#define TAG "Gauge"

// See 5.3 I2C Command Waiting Time
// expected to be 66us, fails till we get to 125us
#define BQ27220_CMD_DELAY_US (250u)

static uint16_t bq27220_read_word(FuriHalI2cBusHandle* handle, uint8_t address) {
    uint16_t buf = 0;

    furi_hal_i2c_read_mem(
        handle, BQ27220_ADDRESS, address, (uint8_t*)&buf, 2, BQ27220_I2C_TIMEOUT);

    furi_delay_us(BQ27220_CMD_DELAY_US);

    return buf;
}

static bool bq27220_write(
    FuriHalI2cBusHandle* handle,
    uint8_t address,
    const uint8_t* buffer,
    size_t buffer_size) {
    bool ret = true;

    for(size_t i = 0; i < buffer_size && ret; i++) {
        ret &= furi_hal_i2c_write_reg_8(
            handle, BQ27220_ADDRESS, address + i, buffer[i], BQ27220_I2C_TIMEOUT);
        furi_delay_us(BQ27220_CMD_DELAY_US);
    }

    return ret;
}

static bool bq27220_control(FuriHalI2cBusHandle* handle, uint16_t control) {
    return bq27220_write(handle, CommandControl, (uint8_t*)&control, 2);
}

static bool bq27220_read(FuriHalI2cBusHandle* handle, uint8_t* buffer, size_t buffer_size) {
    return furi_hal_i2c_rx(handle, BQ27220_ADDRESS, buffer, buffer_size, BQ27220_I2C_TIMEOUT);
}

static uint8_t bq27220_get_checksum(uint8_t* data, uint16_t len) {
    uint8_t ret = 0;
    for(uint16_t i = 0; i < len; i++) {
        ret += data[i];
    }
    return 0xFF - ret;
}

static bool bq27220_parameter_check(
    FuriHalI2cBusHandle* handle,
    uint16_t address,
    uint32_t value,
    size_t size,
    bool update) {
    furi_assert(size == 1 || size == 2 || size == 4);
    bool ret = false;
    uint8_t buffer[6] = {0};
    uint8_t old_data[4] = {0};

    do {
        buffer[0] = address & 0xFF;
        buffer[1] = (address >> 8) & 0xFF;

        for(size_t i = 0; i < size; i++) {
            buffer[1 + size - i] = (value >> (i * 8)) & 0xFF;
        }

        if(update) {
            // Datasheet contains incorrect procedure for memory update, more info:
            // https://e2e.ti.com/support/power-management-group/power-management/f/power-management-forum/719878/bq27220-technical-reference-manual-sluubd4-is-missing-extended-data-commands-chapter

            // 2. Write the address AND the parameter data to 0x3E+ (auto increment)
            if(!bq27220_write(handle, CommandSelectSubclass, buffer, size + 2)) {
                FURI_LOG_I(TAG, "DM write failed");
                break;
            }

            furi_delay_us(10000);

            // 3. Calculate the check sum: 0xFF - (sum of address and data) OR 0xFF
            uint8_t checksum = bq27220_get_checksum(buffer, size + 2);
            // 4. Write the check sum to 0x60 and the total length of (address + parameter data + check sum + length) to 0x61
            buffer[0] = checksum;
            // 2 bytes address, `size` bytes data, 1 byte check sum, 1 byte length
            buffer[1] = 2 + size + 1 + 1;
            if(!bq27220_write(handle, CommandMACDataSum, buffer, 2)) {
                FURI_LOG_I(TAG, "CRC write failed");
                break;
            }

            furi_delay_us(10000);
            ret = true;
        } else {
            if(!bq27220_write(handle, CommandSelectSubclass, buffer, 2)) {
                FURI_LOG_I(TAG, "DM SelectSubclass for read failed");
                break;
            }

            furi_delay_us(BQ27220_CMD_DELAY_US);

            if(!bq27220_read(handle, old_data, size)) {
                FURI_LOG_I(TAG, "DM read failed");
                break;
            }

            furi_delay_us(BQ27220_CMD_DELAY_US);

            if(*(uint32_t*)&(old_data[0]) != *(uint32_t*)&(buffer[2])) {
                FURI_LOG_W( //-V641
                    TAG,
                    "Data at 0x%04x(%zu): 0x%08lx!=0x%08lx",
                    address,
                    size,
                    *(uint32_t*)&(old_data[0]),
                    *(uint32_t*)&(buffer[2]));
            } else {
                ret = true;
            }
        }
    } while(0);

    return ret;
}

static bool bq27220_data_memory_check(
    FuriHalI2cBusHandle* handle,
    const BQ27220DMData* data_memory,
    bool update) {
    if(update) {
        if(!bq27220_control(handle, Control_ENTER_CFG_UPDATE)) {
            FURI_LOG_E(TAG, "ENTER_CFG_UPDATE command failed");
            return false;
        };

        // Wait for enter CFG update mode
        uint32_t timeout = 100;
        Bq27220OperationStatus status = {0};
        while(status.CFGUPDATE != true && status.SEC != Bq27220OperationStatusSecFull && (timeout-- > 0)) {
            bq27220_get_operation_status(handle, &status);
        }

        if(timeout == 0) {
            FURI_LOG_E(TAG, "Enter CFGUPDATE mode failed");
            return false;
        }
    }

    // Process data memory records
    bool result = true;
    while(data_memory->type != BQ27220DMTypeEnd) {
        if(data_memory->type == BQ27220DMTypeWait) {
            furi_delay_us(data_memory->value.u32);
        } else if(data_memory->type == BQ27220DMTypeU8) {
            result &= bq27220_parameter_check(
                handle, data_memory->address, data_memory->value.u8, 1, update);
        } else if(data_memory->type == BQ27220DMTypeU16) {
            result &= bq27220_parameter_check(
                handle, data_memory->address, data_memory->value.u16, 2, update);
        } else if(data_memory->type == BQ27220DMTypeU32) {
            result &= bq27220_parameter_check(
                handle, data_memory->address, data_memory->value.u32, 4, update);
        } else if(data_memory->type == BQ27220DMTypeI8) {
            result &= bq27220_parameter_check(
                handle, data_memory->address, data_memory->value.i8, 1, update);
        } else if(data_memory->type == BQ27220DMTypeI16) {
            result &= bq27220_parameter_check(
                handle, data_memory->address, data_memory->value.i16, 2, update);
        } else if(data_memory->type == BQ27220DMTypeI32) {
            result &= bq27220_parameter_check(
                handle, data_memory->address, data_memory->value.i32, 4, update);
        } else if(data_memory->type == BQ27220DMTypeF32) {
            result &= bq27220_parameter_check(
                handle, data_memory->address, data_memory->value.u32, 4, update);
        } else if(data_memory->type == BQ27220DMTypePtr8) {
            result &= bq27220_parameter_check(
                handle, data_memory->address, *(uint8_t*)data_memory->value.u32, 1, update);
        } else if(data_memory->type == BQ27220DMTypePtr16) {
            result &= bq27220_parameter_check(
                handle, data_memory->address, *(uint16_t*)data_memory->value.u32, 2, update);
        } else if(data_memory->type == BQ27220DMTypePtr32) {
            result &= bq27220_parameter_check(
                handle, data_memory->address, *(uint32_t*)data_memory->value.u32, 4, update);
        } else {
            furi_crash("Invalid DM Type");
        }
        data_memory++;
    }

    // Finalize configuration update
    if(update) {
        bq27220_control(handle, Control_EXIT_CFG_UPDATE_REINIT);
        uint32_t timeout = 100;
        Bq27220OperationStatus status = {0};
        while(status.CFGUPDATE == true && (timeout-- > 0)) {
            bq27220_get_operation_status(handle, &status);
        }
        if(timeout == 0) {
            FURI_LOG_E(TAG, "Exit CFGUPDATE mode failed");
            return false;
        }
    }

    return result;
}

bool bq27220_init(FuriHalI2cBusHandle* handle, const BQ27220DMData* data_memory) {
    Bq27220OperationStatus status = {0};
    bool result = false;

    do {
        // Unseal device since we are going to read protected configuration
        FURI_LOG_D(TAG, "Unsealing");
        if(!bq27220_unseal(handle)) {
            break;
        }

        // Request device number(chip PN)
        FURI_LOG_D(TAG, "Checking device ID");
        if(!bq27220_control(handle, Control_DEVICE_NUMBER)) {
            FURI_LOG_E(TAG, "Device is not responding");
            break;
        };
        // Enterprise wait(MAC read fails if less than 500us)
        furi_delay_us(999);
        // Read id data from MAC scratch space
        uint16_t data = bq27220_read_word(handle, CommandMACData);
        if(data != 0x0220) {
            FURI_LOG_E(TAG, "Invalid Device Number %04x == 0x0220", data);
            break;
        }

        // Get full access to read and modify parameters
        FURI_LOG_D(TAG, "Acquiring Full Access");
        if(!bq27220_full_access(handle)) {
            break;
        }

        if(!bq27220_get_operation_status(handle, &status)) {
            FURI_LOG_E(TAG, "Failed to get operation status");
            break;
        }

        // Once upon
        if(status.INITCOMP != true) {
            FURI_LOG_W(TAG, "Device initialization is incomplete, trying to reset");
            if(!bq27220_reset(handle)) {
                FURI_LOG_E(TAG, "Failed to reset incompletely initialized device");
                break;
            }

            // Ensure full access after reset
            if(!bq27220_full_access(handle)) {
                break;
            }
        }

        FURI_LOG_D(TAG, "Checking data memory");
        if(!bq27220_data_memory_check(handle, data_memory, false)) {
            FURI_LOG_W(TAG, "Updating data memory");
            bq27220_control(handle, Control_SET_PROFILE_1);
            bq27220_data_memory_check(handle, data_memory, true);
            if(!bq27220_data_memory_check(handle, data_memory, false)){
                FURI_LOG_E(TAG, "Data memory update failed");
                break;
            }
        }

        FURI_LOG_D(TAG, "Sealing");
        if(!bq27220_seal(handle)) {
            FURI_LOG_E(TAG, "Seal failed");
            break;
        }

        result = true;
    } while(0);

    return result;
}

bool bq27220_reset(FuriHalI2cBusHandle* handle) {
    bool result = false;
    if(bq27220_control(handle, Control_RESET)) {
        uint32_t timeout = 100;
        Bq27220OperationStatus status = {0};
        while((status.INITCOMP != true) && (timeout-- > 0)) {
            bq27220_get_operation_status(handle, &status);
        }
        if(!timeout) {
            FURI_LOG_E(TAG, "INITCOMP timeout after reset");
        } else {
            result = true;
        }
    } else {
        FURI_LOG_E(TAG, "reset request failed");
    };
    return result;
}

bool bq27220_seal(FuriHalI2cBusHandle* handle) {
    Bq27220OperationStatus status = {0};
    bool result = false;
    do {
        if(!bq27220_get_operation_status(handle, &status)) {
            FURI_LOG_E(TAG, "status query failed");
            break;
        }
        if(status.SEC == Bq27220OperationStatusSecSealed) {
            result = true;
            break;
        }

        if(!bq27220_control(handle, Control_SEALED)) {
            FURI_LOG_E(TAG, "seal request failed");
            break;
        }

        furi_delay_us(999);

        if(!bq27220_get_operation_status(handle, &status)) {
            FURI_LOG_E(TAG, "status query failed");
            break;
        }
        if(status.SEC != Bq27220OperationStatusSecSealed) {
            FURI_LOG_E(TAG, "seal failed");
            break;
        }

        result = true;
    } while(0);

    return result;
}

bool bq27220_unseal(FuriHalI2cBusHandle* handle) {
    Bq27220OperationStatus status = {0};
    bool result = false;
    do {
        if(!bq27220_get_operation_status(handle, &status)) {
            FURI_LOG_E(TAG, "status query failed");
            break;
        }
        if(status.SEC != Bq27220OperationStatusSecSealed) {
            result = true;
            break;
        }

        // Hai, Kazuma desu
        bq27220_control(handle, UnsealKey1);
        furi_delay_us(5000);
        bq27220_control(handle, UnsealKey2);
        furi_delay_us(5000);

        if(!bq27220_get_operation_status(handle, &status)) {
            FURI_LOG_E(TAG, "status query failed");
            break;
        }
        if(status.SEC != Bq27220OperationStatusSecUnsealed) {
            FURI_LOG_E(TAG, "unseal failed %u", status.SEC);
            break;
        }

        result = true;
    } while(0);

    return result;
}

bool bq27220_full_access(FuriHalI2cBusHandle* handle) {
    Bq27220OperationStatus status = {0};
    bool result = false;
    do {
        if(!bq27220_get_operation_status(handle, &status)) {
            FURI_LOG_E(TAG, "status query failed");
            break;
        }
        // Already full access
        if(status.SEC == Bq27220OperationStatusSecFull) {
            result = true;
            break;
        }
        // Must be unsealed to get full access
        if(status.SEC != Bq27220OperationStatusSecUnsealed) {
            FURI_LOG_E(TAG, "not in unsealed state");
            break;
        }

        // Explosion!!!
        bq27220_control(handle, FullAccessKey); //-V760
        furi_delay_us(5000);
        bq27220_control(handle, FullAccessKey);
        furi_delay_us(5000);

        if(!bq27220_get_operation_status(handle, &status)) {
            FURI_LOG_E(TAG, "status query failed");
            break;
        }
        if(status.SEC != Bq27220OperationStatusSecFull) {
            FURI_LOG_E(TAG, "full access failed %u", status.SEC);
            break;
        }

        result = true;
    } while(0);

    return result;
}

uint16_t bq27220_get_voltage(FuriHalI2cBusHandle* handle) {
    return bq27220_read_word(handle, CommandVoltage);
}

int16_t bq27220_get_current(FuriHalI2cBusHandle* handle) {
    return bq27220_read_word(handle, CommandCurrent);
}

bool bq27220_get_battery_status(FuriHalI2cBusHandle* handle, Bq27220BatteryStatus* battery_status) {
    uint16_t data = bq27220_read_word(handle, CommandBatteryStatus);
    if(data == BQ27220_ERROR) {
        return false;
    } else {
        *(uint16_t*)battery_status = data;
        return true;
    }
}

bool bq27220_get_operation_status(
    FuriHalI2cBusHandle* handle,
    Bq27220OperationStatus* operation_status) {
    uint16_t data = bq27220_read_word(handle, CommandOperationStatus);
    if(data == BQ27220_ERROR) {
        return false;
    } else {
        *(uint16_t*)operation_status = data;
        return true;
    }
}

uint16_t bq27220_get_temperature(FuriHalI2cBusHandle* handle) {
    return bq27220_read_word(handle, CommandTemperature);
}

uint16_t bq27220_get_full_charge_capacity(FuriHalI2cBusHandle* handle) {
    return bq27220_read_word(handle, CommandFullChargeCapacity);
}

uint16_t bq27220_get_design_capacity(FuriHalI2cBusHandle* handle) {
    return bq27220_read_word(handle, CommandDesignCapacity);
}

uint16_t bq27220_get_remaining_capacity(FuriHalI2cBusHandle* handle) {
    return bq27220_read_word(handle, CommandRemainingCapacity);
}

uint16_t bq27220_get_state_of_charge(FuriHalI2cBusHandle* handle) {
    return bq27220_read_word(handle, CommandStateOfCharge);
}

uint16_t bq27220_get_state_of_health(FuriHalI2cBusHandle* handle) {
    return bq27220_read_word(handle, CommandStateOfHealth);
}
