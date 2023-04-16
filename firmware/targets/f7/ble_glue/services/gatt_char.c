#include "gatt_char.h"

#include <furi.h>

#define TAG "GattChar"

void flipper_gatt_characteristic_init(
    uint16_t svc_handle,
    const FlipperGattCharacteristicParams* char_descriptor,
    FlipperGattCharacteristicInstance* char_instance) {
    furi_assert(char_descriptor);
    furi_assert(char_instance);

    char_instance->characteristic = char_descriptor;

    uint8_t const* char_data = NULL;
    uint16_t char_data_size = 0;
    bool release_data = false;
    if(char_descriptor->data_prop_type == FlipperGattCharacteristicDataPropsFixed) {
        char_data = char_descriptor->data_prop.fixed.ptr;
        char_data_size = char_descriptor->data_prop.fixed.length;
    } else if(char_descriptor->data_prop_type == FlipperGattCharacteristicDataPropsCallback) {
        release_data = char_descriptor->data_prop.callback.fn(
            char_descriptor->data_prop.callback.context, &char_data, &char_data_size);
    }

    tBleStatus status = aci_gatt_add_char(
        svc_handle,
        char_descriptor->uuid_type,
        &char_descriptor->uuid,
        char_data_size,
        char_descriptor->char_properties,
        char_descriptor->security_permissions,
        char_descriptor->gatt_evt_mask,
        10,
        char_descriptor->is_variable,
        &char_instance->handle);
    if(status) {
        FURI_LOG_E(TAG, "Failed to add %s char: %d", char_descriptor->name, status);
    }
    if(release_data) {
        free((void*)char_data);
    }

    char_instance->descriptor_handle = 0;
    if((status == 0) && char_descriptor->descriptor_params) {
        const FlipperGattCharacteristicDescriptorParams* char_data_descriptor =
            char_descriptor->descriptor_params;
        release_data = char_data_descriptor->value_callback.fn(
            char_data_descriptor->value_callback.context, &char_data, &char_data_size);

        status = aci_gatt_add_char_desc(
            svc_handle,
            char_instance->handle,
            char_data_descriptor->uuid_type,
            &char_data_descriptor->uuid,
            char_data_descriptor->max_length,
            char_data_size,
            char_data,
            char_data_descriptor->security_permissions,
            char_data_descriptor->access_permissions,
            char_data_descriptor->gatt_evt_mask,
            7,
            char_data_descriptor->is_variable,
            &char_instance->descriptor_handle);
        if(status) {
            FURI_LOG_E(TAG, "Failed to add %s char descriptor: %d", char_descriptor->name, status);
        }
        if(release_data) {
            free((void*)char_data);
        }
    }
}

void flipper_gatt_characteristic_delete(
    uint16_t svc_handle,
    FlipperGattCharacteristicInstance* char_instance) {
    tBleStatus status = aci_gatt_del_char(svc_handle, char_instance->handle);
    if(status) {
        FURI_LOG_E(
            TAG, "Failed to delete %s char: %d", char_instance->characteristic->name, status);
    }
}

bool flipper_gatt_characteristic_update(
    uint16_t svc_handle,
    FlipperGattCharacteristicInstance* char_instance,
    const void* source) {
    furi_assert(char_instance);
    const FlipperGattCharacteristicParams* char_descriptor = char_instance->characteristic;
    FURI_LOG_D(TAG, "Updating %s char", char_descriptor->name);

    uint8_t const* char_data = NULL;
    uint16_t char_data_size = 0;
    if(char_descriptor->data_prop_type == FlipperGattCharacteristicDataPropsFixed) {
        char_data = char_descriptor->data_prop.fixed.ptr;
        if(source) {
            char_data = (uint8_t*)source;
        }
        char_data_size = char_descriptor->data_prop.fixed.length;
    } else if(char_descriptor->data_prop_type == FlipperGattCharacteristicDataPropsCallback) {
        const void* context = char_descriptor->data_prop.callback.context;
        if(source) {
            context = source;
        }
        char_descriptor->data_prop.callback.fn(context, &char_data, &char_data_size);
    }

    tBleStatus result = aci_gatt_update_char_value(
        svc_handle, char_instance->handle, 0, char_data_size, char_data);
    if(result) {
        FURI_LOG_E(TAG, "Failed updating %s characteristic: %d", char_descriptor->name, result);
    }
    return result != BLE_STATUS_SUCCESS;
}