#pragma once

#include "gatt_char.h"

#include <furi.h>

static inline tBleStatus aci_gatt_add_char_(
    uint16_t Service_Handle,
    uint8_t Char_UUID_Type,
    const Char_UUID_t* Char_UUID,
    uint16_t Char_Value_Length,
    uint8_t Char_Properties,
    uint8_t Security_Permissions,
    uint8_t GATT_Evt_Mask,
    uint8_t Enc_Key_Size,
    uint8_t Is_Variable,
    uint16_t* Char_Handle) {
    // Log all UUID bytes
    FURI_LOG_I(
        "ACI",
        "aci_gatt_add_char UUID: %02X %02x %02x %02x %02X %02x %02x %02x %02X %02x %02x %02x %02X %02x %02x %02x",
        Char_UUID->Char_UUID_128[0],
        Char_UUID->Char_UUID_128[1],
        Char_UUID->Char_UUID_128[2],
        Char_UUID->Char_UUID_128[3],
        Char_UUID->Char_UUID_128[4],
        Char_UUID->Char_UUID_128[5],
        Char_UUID->Char_UUID_128[6],
        Char_UUID->Char_UUID_128[7],
        Char_UUID->Char_UUID_128[8],
        Char_UUID->Char_UUID_128[9],
        Char_UUID->Char_UUID_128[10],
        Char_UUID->Char_UUID_128[11],
        Char_UUID->Char_UUID_128[12],
        Char_UUID->Char_UUID_128[13],
        Char_UUID->Char_UUID_128[14],
        Char_UUID->Char_UUID_128[15]);

    // Log all arguments
    FURI_LOG_I(
        "ACI",
        "aci_gatt_add_char: Service_Handle=%d,  Char_Value_Length = %d, Char_Properties = %d, Security_Permissions = %d, GATT_Evt_Mask = %d, Enc_Key_Size = %d, Is_Variable = %d, Char_Handle = %p ",
        Service_Handle,
        Char_Value_Length,
        Char_Properties,
        Security_Permissions,
        GATT_Evt_Mask,
        Enc_Key_Size,
        Is_Variable,
        Char_Handle);
    return aci_gatt_add_char(
        Service_Handle,
        Char_UUID_Type,
        Char_UUID,
        Char_Value_Length,
        Char_Properties,
        Security_Permissions,
        GATT_Evt_Mask,
        Enc_Key_Size,
        Is_Variable,
        Char_Handle);
}

static inline tBleStatus aci_gatt_add_char_desc_(
    uint16_t Service_Handle,
    uint16_t Char_Handle,
    uint8_t Char_Desc_Uuid_Type,
    const Char_Desc_Uuid_t* Char_Desc_Uuid,
    uint8_t Char_Desc_Value_Max_Len,
    uint8_t Char_Desc_Value_Length,
    const uint8_t* Char_Desc_Value,
    uint8_t Security_Permissions,
    uint8_t Access_Permissions,
    uint8_t GATT_Evt_Mask,
    uint8_t Enc_Key_Size,
    uint8_t Is_Variable,
    uint16_t* Char_Desc_Handle) {
    // Log all UUID bytes
    FURI_LOG_I(
        "ACI",
        "aci_gatt_add_char_desc UUID: %02X %02x %02x %02x %02X %02x %02x %02x %02X %02x %02x %02x %02X %02x %02x %02x",
        Char_Desc_Uuid->Char_UUID_128[0],
        Char_Desc_Uuid->Char_UUID_128[1],
        Char_Desc_Uuid->Char_UUID_128[2],
        Char_Desc_Uuid->Char_UUID_128[3],
        Char_Desc_Uuid->Char_UUID_128[4],
        Char_Desc_Uuid->Char_UUID_128[5],
        Char_Desc_Uuid->Char_UUID_128[6],
        Char_Desc_Uuid->Char_UUID_128[7],
        Char_Desc_Uuid->Char_UUID_128[8],
        Char_Desc_Uuid->Char_UUID_128[9],
        Char_Desc_Uuid->Char_UUID_128[10],
        Char_Desc_Uuid->Char_UUID_128[11],
        Char_Desc_Uuid->Char_UUID_128[12],
        Char_Desc_Uuid->Char_UUID_128[13],
        Char_Desc_Uuid->Char_UUID_128[14],
        Char_Desc_Uuid->Char_UUID_128[15]);
    // Log all arguments
    FURI_LOG_I(
        "ACI",
        "aci_gatt_add_char_desc: Service_Handle=%d, Char_Handle=%d, Char_Desc_Uuid_Type=%d, Char_Desc_Value_Max_Len=%d, Char_Desc_Value_Length=%d, Security_Permissions=%d, Access_Permissions=%d, GATT_Evt_Mask=%d, Enc_Key_Size=%d, Is_Variable=%d, Char_Desc_Handle=%p",
        Service_Handle,
        Char_Handle,
        Char_Desc_Uuid_Type,
        Char_Desc_Value_Max_Len,
        Char_Desc_Value_Length,
        Security_Permissions,
        Access_Permissions,
        GATT_Evt_Mask,
        Enc_Key_Size,
        Is_Variable,
        Char_Desc_Handle);
    return aci_gatt_add_char_desc(
        Service_Handle,
        Char_Handle,
        Char_Desc_Uuid_Type,
        Char_Desc_Uuid,
        Char_Desc_Value_Max_Len,
        Char_Desc_Value_Length,
        Char_Desc_Value,
        Security_Permissions,
        Access_Permissions,
        GATT_Evt_Mask,
        Enc_Key_Size,
        Is_Variable,
        Char_Desc_Handle);
}

static inline tBleStatus aci_gatt_update_char_value_(
    uint16_t Service_Handle,
    uint16_t Char_Handle,
    uint8_t Val_Offset,
    uint8_t Char_Value_Length,
    const uint8_t* Char_Value) {
    FURI_LOG_I(
        "ACI",
        "aci_gatt_update_char_value: Service_Handle=%d, Char_Handle=%d, Val_Offset=%d, Char_Value_Length=%d",
        Service_Handle,
        Char_Handle,
        Val_Offset,
        Char_Value_Length);
    return aci_gatt_update_char_value(
        Service_Handle, Char_Handle, Val_Offset, Char_Value_Length, Char_Value);
}