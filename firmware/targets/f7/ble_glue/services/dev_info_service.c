#include "dev_info_service.h"
#include "app_common.h"
#include "gatt_char.h"
#include <ble/ble.h>

#include <furi.h>
#include <protobuf_version.h>
#include <lib/toolbox/version.h>

#define TAG "BtDevInfoSvc"

typedef enum {
    DevInfoSvcGattCharacteristicMfgName = 0,
    DevInfoSvcGattCharacteristicSerial,
    DevInfoSvcGattCharacteristicFirmwareRev,
    DevInfoSvcGattCharacteristicSoftwareRev,
    DevInfoSvcGattCharacteristicRpcVersion,
    DevInfoSvcGattCharacteristicCount,
} DevInfoSvcGattCharacteristicId;

typedef struct {
    uint16_t service_handle;
    FlipperGattCharacteristicInstance characteristics[DevInfoSvcGattCharacteristicCount];
    FuriString* version_string;
    char hardware_revision[4];
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
    *data = (const uint8_t*)&dev_info_svc->hardware_revision;
    *data_len = sizeof(dev_info_svc->hardware_revision);
    return false;
}

static bool dev_info_char_software_rev_callback(
    const void* context,
    const uint8_t** data,
    uint16_t* data_len) {
    const DevInfoSvc* dev_info_svc = *(DevInfoSvc**)context;
    *data = (const uint8_t*)furi_string_get_cstr(dev_info_svc->version_string);
    *data_len = furi_string_size(dev_info_svc->version_string);
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
         .uuid.Char_UUID_128 =
             {0x33,
              0xa9,
              0xb5,
              0x3e,
              0x87,
              0x5d,
              0x1a,
              0x8e,
              0xc8,
              0x47,
              0x5e,
              0xae,
              0x6d,
              0x66,
              0xf6,
              0x03},
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

    for(size_t i = 0; i < DevInfoSvcGattCharacteristicCount; i++) {
        flipper_gatt_characteristic_init(
            dev_info_svc->service_handle,
            &dev_info_svc_chars[i],
            &dev_info_svc->characteristics[i]);
        flipper_gatt_characteristic_update(
            dev_info_svc->service_handle, &dev_info_svc->characteristics[i], NULL);
    }
}

void dev_info_svc_stop() {
    tBleStatus status;
    if(dev_info_svc) {
        furi_string_free(dev_info_svc->version_string);
        // Delete service characteristics
        for(size_t i = 0; i < DevInfoSvcGattCharacteristicCount; i++) {
            flipper_gatt_characteristic_delete(
                dev_info_svc->service_handle, &dev_info_svc->characteristics[i]);
        }
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
