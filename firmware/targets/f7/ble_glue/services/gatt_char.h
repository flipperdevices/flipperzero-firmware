#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <ble/ble.h>

#ifdef __cplusplus
extern "C" {
#endif

// returns true if you own the data pointer now and must free() it
typedef bool (*FlipperGattCharacteristicPropsCallback)(
    const void* context,
    const uint8_t** data,
    uint16_t* data_len);

typedef enum {
    FlipperGattCharacteristicDataPropsFixed,
    FlipperGattCharacteristicDataPropsCallback,
} FlipperGattCharacteristicDataType;

typedef struct {
    Char_Desc_Uuid_t uuid;
    uint8_t uuid_type;
    uint8_t max_length;
    struct {
        FlipperGattCharacteristicPropsCallback fn;
        const void* context;
    } value_callback;
    uint8_t security_permissions;
    uint8_t access_permissions;
    uint8_t evt_mask;
    uint8_t is_variable;
} FlipperGattCharacteristicDescriptorParams;

typedef struct {
    const char* name;
    FlipperGattCharacteristicDescriptorParams* descriptor_params;
    FlipperGattCharacteristicDataType data_prop_type;
    union {
        struct {
            const uint8_t* ptr;
            uint16_t length;
        } fixed;
        struct {
            FlipperGattCharacteristicPropsCallback fn;
            const void* context;
        } callback;
    } data_prop;
    Char_UUID_t uuid;
    uint8_t uuid_type;
    uint8_t properties;
    uint8_t permissions;
    uint8_t evt_mask;
    uint8_t is_variable;
} FlipperGattCharacteristicParams;

typedef struct {
    const FlipperGattCharacteristicParams* characteristic;
    uint16_t handle;
    uint16_t descriptor_handle;
} FlipperGattCharacteristicInstance;

bool flipper_gatt_characteristic_props_const_char(
    const void* context,
    const uint8_t** data,
    uint16_t* data_len);

void flipper_gatt_characteristic_init(
    uint16_t svc_handle,
    const FlipperGattCharacteristicParams* char_descriptor,
    FlipperGattCharacteristicInstance* char_instance);

void flipper_gatt_characteristic_deinit(
    uint16_t svc_handle,
    FlipperGattCharacteristicInstance* char_instance);

bool flipper_gatt_characteristic_update(
    uint16_t svc_handle,
    FlipperGattCharacteristicInstance* char_instance,
    const void* source);

#ifdef __cplusplus
}
#endif