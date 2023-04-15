#include "hid_service.h"
#include "app_common.h"
#include <ble/ble.h>
#include "gatt_char.h"

#include <furi.h>

#define TAG "BtHid"

typedef enum {
    HidSvcGattCharacteristicProtocolMode = 0,
    HidSvcGattCharacteristicReportMap,
    HidSvcGattCharacteristicInfo,
    HidSvcGattCharacteristicCtrlPoint,
    HidSvcGattCharacteristicCount,
} HidSvcGattCharacteristicId;

typedef struct {
    uint8_t report_idx;
    uint8_t report_type;
} HidSvcReportId;

static_assert(sizeof(HidSvcReportId) == sizeof(uint16_t), "HidSvcReportId must be 2 bytes");

bool hid_svc_char_desc_data_callback(const void* context, const uint8_t** data, uint16_t* data_len) {
    const HidSvcReportId* report_id = context;
    *data = (const uint8_t*)report_id;
    *data_len = sizeof(HidSvcReportId);
    return false;
}

static const FlipperGattCharacteristicParams hid_svc_chars[HidSvcGattCharacteristicCount] = {
    [HidSvcGattCharacteristicProtocolMode] =
        {.name = "Protocol Mode",
         .data_prop_type = FlipperGattCharacteristicDataPropsFixed,
         .data_prop.fixed.length = 1,
         .uuid.Char_UUID_16 = PROTOCOL_MODE_CHAR_UUID,
         .uuid_type = UUID_TYPE_16,
         .char_properties = CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RESP,
         .security_permissions = ATTR_PERMISSION_NONE,
         .gatt_evt_mask = GATT_NOTIFY_ATTRIBUTE_WRITE,
         .is_variable = CHAR_VALUE_LEN_CONSTANT},
    [HidSvcGattCharacteristicReportMap] =
        {.name = "Report Map",
         .data_prop_type = FlipperGattCharacteristicDataPropsFixed,
         .data_prop.fixed.length = HID_SVC_REPORT_MAP_MAX_LEN,
         .uuid.Char_UUID_16 = REPORT_MAP_CHAR_UUID,
         .uuid_type = UUID_TYPE_16,
         .char_properties = CHAR_PROP_READ,
         .security_permissions = ATTR_PERMISSION_NONE,
         .gatt_evt_mask = GATT_DONT_NOTIFY_EVENTS,
         .is_variable = CHAR_VALUE_LEN_VARIABLE},
    [HidSvcGattCharacteristicInfo] =
        {.name = "HID Information",
         .data_prop_type = FlipperGattCharacteristicDataPropsFixed,
         .data_prop.fixed.length = HID_SVC_INFO_LEN,
         .uuid.Char_UUID_16 = HID_INFORMATION_CHAR_UUID,
         .uuid_type = UUID_TYPE_16,
         .char_properties = CHAR_PROP_READ,
         .security_permissions = ATTR_PERMISSION_NONE,
         .gatt_evt_mask = GATT_DONT_NOTIFY_EVENTS,
         .is_variable = CHAR_VALUE_LEN_CONSTANT},
    [HidSvcGattCharacteristicCtrlPoint] =
        {.name = "HID Control Point",
         .data_prop_type = FlipperGattCharacteristicDataPropsFixed,
         .data_prop.fixed.length = HID_SVC_CONTROL_POINT_LEN,
         .uuid.Char_UUID_16 = HID_CONTROL_POINT_CHAR_UUID,
         .uuid_type = UUID_TYPE_16,
         .char_properties = CHAR_PROP_WRITE_WITHOUT_RESP,
         .security_permissions = ATTR_PERMISSION_NONE,
         .gatt_evt_mask = GATT_NOTIFY_ATTRIBUTE_WRITE,
         .is_variable = CHAR_VALUE_LEN_CONSTANT},
};

static const FlipperGattCharacteristicDescriptorParams hid_svc_char_descr_template = {
    .uuid_type = UUID_TYPE_16,
    .uuid.Char_UUID_16 = REPORT_REFERENCE_DESCRIPTOR_UUID,
    .max_length = HID_SVC_REPORT_REF_LEN,
    .value_callback.fn = hid_svc_char_desc_data_callback,
    .security_permissions = ATTR_PERMISSION_NONE,
    .access_permissions = ATTR_ACCESS_READ_WRITE,
    .gatt_evt_mask = GATT_DONT_NOTIFY_EVENTS,
    .is_variable = CHAR_VALUE_LEN_CONSTANT,
};

static const FlipperGattCharacteristicParams hid_svc_report_template = {
    .data_prop_type = FlipperGattCharacteristicDataPropsFixed,
    .data_prop.fixed.length = HID_SVC_REPORT_MAX_LEN,
    .uuid.Char_UUID_16 = REPORT_CHAR_UUID,
    .uuid_type = UUID_TYPE_16,
    .char_properties = CHAR_PROP_READ | CHAR_PROP_NOTIFY,
    .security_permissions = ATTR_PERMISSION_NONE,
    .gatt_evt_mask = GATT_DONT_NOTIFY_EVENTS,
    .is_variable = CHAR_VALUE_LEN_VARIABLE,
};

typedef struct {
    uint16_t svc_handle;
    FlipperGattCharacteristicInstance chars[HidSvcGattCharacteristicCount];
    FlipperGattCharacteristicInstance input_report_chars[HID_SVC_INPUT_REPORT_COUNT];
    FlipperGattCharacteristicInstance output_report_chars[HID_SVC_OUTPUT_REPORT_COUNT];
    FlipperGattCharacteristicInstance feature_report_chars[HID_SVC_FEATURE_REPORT_COUNT];
} HIDSvc;

static HIDSvc* hid_svc = NULL;

static SVCCTL_EvtAckStatus_t hid_svc_event_handler(void* event) {
    SVCCTL_EvtAckStatus_t ret = SVCCTL_EvtNotAck;
    hci_event_pckt* event_pckt = (hci_event_pckt*)(((hci_uart_pckt*)event)->data);
    evt_blecore_aci* blecore_evt = (evt_blecore_aci*)event_pckt->data;
    // aci_gatt_attribute_modified_event_rp0* attribute_modified;
    if(event_pckt->evt == HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE) {
        if(blecore_evt->ecode == ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE) {
            // Process modification events
            ret = SVCCTL_EvtAckFlowEnable;
        } else if(blecore_evt->ecode == ACI_GATT_SERVER_CONFIRMATION_VSEVT_CODE) {
            // Process notification confirmation
            ret = SVCCTL_EvtAckFlowEnable;
        }
    }
    return ret;
}

void hid_svc_start() {
    tBleStatus status;
    hid_svc = malloc(sizeof(HIDSvc));
    Service_UUID_t svc_uuid = {};

    // Register event handler
    SVCCTL_RegisterSvcHandler(hid_svc_event_handler);
    // Add service
    svc_uuid.Service_UUID_16 = HUMAN_INTERFACE_DEVICE_SERVICE_UUID;
    /**
     *  Add Human Interface Device Service
     */
    status = aci_gatt_add_service(
        UUID_TYPE_16,
        &svc_uuid,
        PRIMARY_SERVICE,
        2 + /* protocol mode */
            (4 * HID_SVC_INPUT_REPORT_COUNT) + (3 * HID_SVC_OUTPUT_REPORT_COUNT) +
            (3 * HID_SVC_FEATURE_REPORT_COUNT) + 1 + 2 + 2 +
            2, /* Service + Report Map + HID Information + HID Control Point */
        &hid_svc->svc_handle);
    if(status) {
        FURI_LOG_E(TAG, "Failed to add HID service: %d", status);
    }

    for(size_t i = 0; i < HidSvcGattCharacteristicCount; i++) {
        flipper_gatt_characteristic_init(
            hid_svc->svc_handle, &hid_svc_chars[i], &hid_svc->chars[i]);
    }
    uint8_t protocol_mode = 1;
    flipper_gatt_characteristic_update(
        hid_svc->svc_handle,
        &hid_svc->chars[HidSvcGattCharacteristicProtocolMode],
        &protocol_mode);

    // reports
    FlipperGattCharacteristicDescriptorParams hid_svc_char_descr;
    FlipperGattCharacteristicParams report_char;
    HidSvcReportId report_id;

    memcpy(&hid_svc_char_descr, &hid_svc_char_descr_template, sizeof(hid_svc_char_descr));
    memcpy(&report_char, &hid_svc_report_template, sizeof(report_char));

    hid_svc_char_descr.value_callback.context = &report_id;
    report_char.descriptor_params = &hid_svc_char_descr;

    typedef struct {
        uint8_t report_type;
        uint8_t report_count;
        FlipperGattCharacteristicInstance* chars;
    } HidSvcReportCharProps;

    HidSvcReportCharProps hid_report_chars[] = {
        {0x01, HID_SVC_INPUT_REPORT_COUNT, hid_svc->input_report_chars},
        {0x02, HID_SVC_OUTPUT_REPORT_COUNT, hid_svc->output_report_chars},
        {0x03, HID_SVC_FEATURE_REPORT_COUNT, hid_svc->feature_report_chars},
    };

    for(size_t i = 0; i < COUNT_OF(hid_report_chars); i++) {
        report_id.report_type = hid_report_chars[i].report_type;
        for(size_t j = 0; j < hid_report_chars[i].report_count; j++) {
            report_id.report_idx = j + 1;
            flipper_gatt_characteristic_init(
                hid_svc->svc_handle, &report_char, &hid_report_chars[i].chars[j]);
        }
    }
}

bool hid_svc_update_report_map(const uint8_t* data, uint16_t len) {
    furi_assert(data);
    furi_assert(hid_svc);

    tBleStatus status = aci_gatt_update_char_value(
        hid_svc->svc_handle,
        hid_svc->chars[HidSvcGattCharacteristicReportMap].handle,
        0,
        len,
        data);
    if(status) {
        FURI_LOG_E(TAG, "Failed updating report map characteristic: %d", status);
        return false;
    }
    return true;
}

bool hid_svc_update_input_report(uint8_t input_report_num, uint8_t* data, uint16_t len) {
    furi_assert(data);
    furi_assert(hid_svc);
    furi_check(input_report_num < HID_SVC_INPUT_REPORT_COUNT);

    tBleStatus status = aci_gatt_update_char_value(
        hid_svc->svc_handle, hid_svc->input_report_chars[input_report_num].handle, 0, len, data);
    if(status) {
        FURI_LOG_E(TAG, "Failed updating report characteristic: %d", status);
        return false;
    }
    return true;
}

bool hid_svc_update_info(uint8_t* data, uint16_t len) {
    furi_assert(data);
    furi_assert(hid_svc);

    tBleStatus status = aci_gatt_update_char_value(
        hid_svc->svc_handle, hid_svc->chars[HidSvcGattCharacteristicInfo].handle, 0, len, data);
    if(status) {
        FURI_LOG_E(TAG, "Failed updating info characteristic: %d", status);
        return false;
    }
    return true;
}

bool hid_svc_is_started() {
    return hid_svc != NULL;
}

void hid_svc_stop() {
    tBleStatus status;
    if(hid_svc) {
        // Delete characteristics
        for(size_t i = 0; i < HidSvcGattCharacteristicCount; i++) {
            flipper_gatt_characteristic_delete(hid_svc->svc_handle, &hid_svc->chars[i]);
        }

        typedef struct {
            uint8_t report_count;
            FlipperGattCharacteristicInstance* chars;
        } HidSvcReportCharProps;

        HidSvcReportCharProps hid_report_chars[] = {
            {HID_SVC_INPUT_REPORT_COUNT, hid_svc->input_report_chars},
            {HID_SVC_OUTPUT_REPORT_COUNT, hid_svc->output_report_chars},
            {HID_SVC_FEATURE_REPORT_COUNT, hid_svc->feature_report_chars},
        };

        for(size_t i = 0; i < COUNT_OF(hid_report_chars); i++) {
            for(size_t j = 0; j < hid_report_chars[i].report_count; j++) {
                flipper_gatt_characteristic_delete(
                    hid_svc->svc_handle, &hid_report_chars[i].chars[j]);
            }
        }

        // Delete service
        status = aci_gatt_del_service(hid_svc->svc_handle);
        if(status) {
            FURI_LOG_E(TAG, "Failed to delete HID service: %d", status);
        }
        free(hid_svc);
        hid_svc = NULL;
    }
}
