#include "dev_info_service.h"
#include "app_common.h"
#include "gatt_char.h"
#include <ble/ble.h>

#include <furi.h>
#include <protobuf_version.h>
#include <lib/toolbox/version.h>

#include "dev_info_service_uuid.inc"

#define TAG "BtDevInfoSvc"

typedef enum {
    DevInfoSvcGattCharacteristicMfgName = 0,
    DevInfoSvcGattCharacteristicSerial,
    DevInfoSvcGattCharacteristicFirmwareRev,
    DevInfoSvcGattCharacteristicSoftwareRev,
    DevInfoSvcGattCharacteristicRpcVersion,
    DevInfoSvcGattCharacteristicCount,
} DevInfoSvcGattCharacteristicId;

#define DEVICE_INFO_HARDWARE_REV_SIZE 4
typedef struct {
    uint16_t service_handle;
    FlipperGattCharacteristicInstance characteristics[DevInfoSvcGattCharacteristicCount];
    FuriString* version_string;
    char hardware_revision[DEVICE_INFO_HARDWARE_REV_SIZE];
    // uint16_t man_name_char_handle;
    // uint16_t serial_num_char_handle;
    // uint16_t firmware_rev_char_handle;
    // uint16_t software_rev_char_handle;
    // uint16_t rpc_version_char_handle;
} DevInfoSvc;

static DevInfoSvc* dev_info_svc = NULL;

static const char dev_info_man_name[] = "Flipper Devices Inc.";
static const char dev_info_serial_num[] = "1.0";
static const char dev_info_rpc_version[] = TOSTRING(PROTOBUF_MAJOR_VERSION.PROTOBUF_MINOR_VERSION);

static bool dev_info_char_firmware_rev_callback(
    const void* context,
    const uint8_t** data,
    uint16_t* data_len) {
    const DevInfoSvc* dev_info_svc = *(DevInfoSvc**)context;
    *data_len = sizeof(dev_info_svc->hardware_revision) - 1;
    if(data) {
        *data = (const uint8_t*)&dev_info_svc->hardware_revision;
    }
    return false;
}

static bool dev_info_char_software_rev_callback(
    const void* context,
    const uint8_t** data,
    uint16_t* data_len) {
    const DevInfoSvc* dev_info_svc = *(DevInfoSvc**)context;
    *data_len = furi_string_size(dev_info_svc->version_string);
    if(data) {
        *data = (const uint8_t*)furi_string_get_cstr(dev_info_svc->version_string);
    }
    return false;
}

static const FlipperGattCharacteristicParams dev_info_svc_chars[DevInfoSvcGattCharacteristicCount] =
    {[DevInfoSvcGattCharacteristicMfgName] =
         {.name = "Manufacturer Name",
          .data_prop_type = FlipperGattCharacteristicDataFixed,
          .data.fixed.length = sizeof(dev_info_man_name) - 1,
          .data.fixed.ptr = (const uint8_t*)&dev_info_man_name,
          .uuid.Char_UUID_16 = MANUFACTURER_NAME_UUID,
          .uuid_type = UUID_TYPE_16,
          .char_properties = CHAR_PROP_READ,
          .security_permissions = ATTR_PERMISSION_AUTHEN_READ,
          .gatt_evt_mask = GATT_DONT_NOTIFY_EVENTS,
          .is_variable = CHAR_VALUE_LEN_CONSTANT},
     [DevInfoSvcGattCharacteristicSerial] =
         {.name = "Serial Number",
          .data_prop_type = FlipperGattCharacteristicDataFixed,
          .data.fixed.length = sizeof(dev_info_serial_num) - 1,
          .data.fixed.ptr = (const uint8_t*)&dev_info_serial_num,
          .uuid.Char_UUID_16 = SERIAL_NUMBER_UUID,
          .uuid_type = UUID_TYPE_16,
          .char_properties = CHAR_PROP_READ,
          .security_permissions = ATTR_PERMISSION_AUTHEN_READ,
          .gatt_evt_mask = GATT_DONT_NOTIFY_EVENTS,
          .is_variable = CHAR_VALUE_LEN_CONSTANT},
     [DevInfoSvcGattCharacteristicFirmwareRev] =
         {.name = "Firmware Revision",
          .data_prop_type = FlipperGattCharacteristicDataCallback,
          .data.callback.context = &dev_info_svc,
          .data.callback.fn = dev_info_char_firmware_rev_callback,
          .uuid.Char_UUID_16 = FIRMWARE_REVISION_UUID,
          .uuid_type = UUID_TYPE_16,
          .char_properties = CHAR_PROP_READ,
          .security_permissions = ATTR_PERMISSION_AUTHEN_READ,
          .gatt_evt_mask = GATT_DONT_NOTIFY_EVENTS,
          .is_variable = CHAR_VALUE_LEN_CONSTANT},
     [DevInfoSvcGattCharacteristicSoftwareRev] =
         {.name = "Software Revision",
          .data_prop_type = FlipperGattCharacteristicDataCallback,
          .data.callback.context = &dev_info_svc,
          .data.callback.fn = dev_info_char_software_rev_callback,
          .uuid.Char_UUID_16 = SOFTWARE_REVISION_UUID,
          .uuid_type = UUID_TYPE_16,
          .char_properties = CHAR_PROP_READ,
          .security_permissions = ATTR_PERMISSION_AUTHEN_READ,
          .gatt_evt_mask = GATT_DONT_NOTIFY_EVENTS,
          .is_variable = CHAR_VALUE_LEN_CONSTANT},
     [DevInfoSvcGattCharacteristicRpcVersion] = {
         .name = "RPC Version",
         .data_prop_type = FlipperGattCharacteristicDataFixed,
         .data.fixed.length = sizeof(dev_info_rpc_version) - 1,
         .data.fixed.ptr = (const uint8_t*)&dev_info_rpc_version,
         .uuid.Char_UUID_128 = DEV_INVO_RPC_VERSION_UID,
         .uuid_type = UUID_TYPE_128,
         .char_properties = CHAR_PROP_READ,
         .security_permissions = ATTR_PERMISSION_AUTHEN_READ,
         .gatt_evt_mask = GATT_DONT_NOTIFY_EVENTS,
         .is_variable = CHAR_VALUE_LEN_CONSTANT}};

void dev_info_svc_start() {
    dev_info_svc = malloc(sizeof(DevInfoSvc));
    dev_info_svc->version_string = furi_string_alloc_printf(
        "%s %s %s %s",
        version_get_githash(NULL),
        version_get_gitbranch(NULL),
        version_get_gitbranchnum(NULL),
        version_get_builddate(NULL));
    snprintf(
        dev_info_svc->hardware_revision,
        sizeof(dev_info_svc->hardware_revision),
        "%d",
        version_get_target(NULL));
    tBleStatus status;

    // Add Device Information Service
    uint16_t uuid = DEVICE_INFORMATION_SERVICE_UUID;
    status = aci_gatt_add_service(
        UUID_TYPE_16,
        (Service_UUID_t*)&uuid,
        PRIMARY_SERVICE,
        1 + 2 * DevInfoSvcGattCharacteristicCount,
        &dev_info_svc->service_handle);
    if(status) {
        FURI_LOG_E(TAG, "Failed to add Device Information Service: %d", status);
    }

    // for(size_t i = 0; i < DevInfoSvcGattCharacteristicCount; i++) {
    //     flipper_gatt_characteristic_init(
    //         dev_info_svc->service_handle,
    //         &dev_info_svc_chars[i],
    //         &dev_info_svc->characteristics[i]);
    // }

    flipper_gatt_characteristic_init(
        dev_info_svc->service_handle,
        &dev_info_svc_chars[DevInfoSvcGattCharacteristicMfgName],
        &dev_info_svc->characteristics[DevInfoSvcGattCharacteristicMfgName]);

    flipper_gatt_characteristic_init(
        dev_info_svc->service_handle,
        &dev_info_svc_chars[DevInfoSvcGattCharacteristicSerial],
        &dev_info_svc->characteristics[DevInfoSvcGattCharacteristicSerial]);

    UNUSED(dev_info_char_software_rev_callback);
    UNUSED(dev_info_char_firmware_rev_callback);

    uuid = FIRMWARE_REVISION_UUID;
    status = aci_gatt_add_char(
        dev_info_svc->service_handle,
        UUID_TYPE_16,
        (Char_UUID_t*)&uuid,
        strlen(dev_info_svc->hardware_revision),
        CHAR_PROP_READ,
        ATTR_PERMISSION_AUTHEN_READ,
        GATT_DONT_NOTIFY_EVENTS,
        10,
        CHAR_VALUE_LEN_CONSTANT,
        &dev_info_svc->characteristics[DevInfoSvcGattCharacteristicFirmwareRev].handle);
    if(status) {
        FURI_LOG_E(TAG, "Failed to add firmware revision char: %d", status);
    }

    uuid = SOFTWARE_REVISION_UUID;
    status = aci_gatt_add_char(
        dev_info_svc->service_handle,
        UUID_TYPE_16,
        (Char_UUID_t*)&uuid,
        furi_string_size(dev_info_svc->version_string),
        CHAR_PROP_READ,
        ATTR_PERMISSION_AUTHEN_READ,
        GATT_DONT_NOTIFY_EVENTS,
        10,
        CHAR_VALUE_LEN_CONSTANT,
        &dev_info_svc->characteristics[DevInfoSvcGattCharacteristicSoftwareRev].handle);
    if(status) {
        FURI_LOG_E(TAG, "Failed to add software revision char: %d", status);
    }

    flipper_gatt_characteristic_init(
        dev_info_svc->service_handle,
        &dev_info_svc_chars[DevInfoSvcGattCharacteristicRpcVersion],
        &dev_info_svc->characteristics[DevInfoSvcGattCharacteristicRpcVersion]);

    // static const uint8_t dev_info_rpc_version_uuid[] = DEV_INVO_RPC_VERSION_UID;
    // status = aci_gatt_add_char(
    //     dev_info_svc->service_handle,
    //     UUID_TYPE_128,
    //     (const Char_UUID_t*)dev_info_rpc_version_uuid,
    //     strlen(dev_info_rpc_version),
    //     CHAR_PROP_READ,
    //     ATTR_PERMISSION_AUTHEN_READ,
    //     GATT_DONT_NOTIFY_EVENTS,
    //     10,
    //     CHAR_VALUE_LEN_CONSTANT,
    //     &dev_info_svc->rpc_version_char_handle);
    // if(status) {
    //     FURI_LOG_E(TAG, "Failed to add rpc version characteristic: %d", status);
    // }

    // for(size_t i = 0; i < DevInfoSvcGattCharacteristicCount; i++) {
    //     flipper_gatt_characteristic_update(
    //         dev_info_svc->service_handle, &dev_info_svc->characteristics[i], NULL);
    // }

    flipper_gatt_characteristic_update(
        dev_info_svc->service_handle,
        &dev_info_svc->characteristics[DevInfoSvcGattCharacteristicMfgName],
        NULL);
    flipper_gatt_characteristic_update(
        dev_info_svc->service_handle,
        &dev_info_svc->characteristics[DevInfoSvcGattCharacteristicSerial],
        NULL);

    status = aci_gatt_update_char_value(
        dev_info_svc->service_handle,
        dev_info_svc->characteristics[DevInfoSvcGattCharacteristicFirmwareRev].handle,
        0,
        strlen(dev_info_svc->hardware_revision),
        (uint8_t*)dev_info_svc->hardware_revision);
    if(status) {
        FURI_LOG_E(TAG, "Failed to update firmware revision char: %d", status);
    }
    status = aci_gatt_update_char_value(
        dev_info_svc->service_handle,
        dev_info_svc->characteristics[DevInfoSvcGattCharacteristicSoftwareRev].handle,
        0,
        furi_string_size(dev_info_svc->version_string),
        (uint8_t*)furi_string_get_cstr(dev_info_svc->version_string));
    if(status) {
        FURI_LOG_E(TAG, "Failed to update software revision char: %d", status);
    }

    flipper_gatt_characteristic_update(
        dev_info_svc->service_handle,
        &dev_info_svc->characteristics[DevInfoSvcGattCharacteristicRpcVersion],
        NULL);

    // status = aci_gatt_update_char_value(
    //     dev_info_svc->service_handle,
    //     dev_info_svc->rpc_version_char_handle,
    //     0,
    //     strlen(dev_info_rpc_version),
    //     (uint8_t*)dev_info_rpc_version);
    // if(status) {
    //     FURI_LOG_E(TAG, "Failed to update rpc version char: %d", status);
    // }
}

void dev_info_svc_stop() {
    tBleStatus status;
    if(dev_info_svc) {
        furi_string_free(dev_info_svc->version_string);
        // Delete service characteristics
        // for(size_t i = 0; i < DevInfoSvcGattCharacteristicCount; i++) {
        //     flipper_gatt_characteristic_delete(
        //         dev_info_svc->service_handle, &dev_info_svc->characteristics[i]);
        // }

        flipper_gatt_characteristic_delete(
            dev_info_svc->service_handle,
            &dev_info_svc->characteristics[DevInfoSvcGattCharacteristicMfgName]);
        flipper_gatt_characteristic_delete(
            dev_info_svc->service_handle,
            &dev_info_svc->characteristics[DevInfoSvcGattCharacteristicSerial]);

        status = aci_gatt_del_char(
            dev_info_svc->service_handle,
            dev_info_svc->characteristics[DevInfoSvcGattCharacteristicFirmwareRev].handle);
        if(status) {
            FURI_LOG_E(TAG, "Failed to delete firmware revision char: %d", status);
        }

        status = aci_gatt_del_char(
            dev_info_svc->service_handle,
            dev_info_svc->characteristics[DevInfoSvcGattCharacteristicSoftwareRev].handle);
        if(status) {
            FURI_LOG_E(TAG, "Failed to delete software revision char: %d", status);
        }

        flipper_gatt_characteristic_delete(
            dev_info_svc->service_handle,
            &dev_info_svc->characteristics[DevInfoSvcGattCharacteristicRpcVersion]);

        // Delete service
        status = aci_gatt_del_service(dev_info_svc->service_handle);
        if(status) {
            FURI_LOG_E(TAG, "Failed to delete device info service: %d", status);
        }
        free(dev_info_svc);
        dev_info_svc = NULL;
    }
}

bool dev_info_svc_is_started() {
    return dev_info_svc != NULL;
}
