#include "nfc_types.h"

const char* nfc_get_dev_type(FuriHalNfcType type) {
    if(type == FuriHalNfcTypeA) {
        return "NFC-A";
    } else if(type == FuriHalNfcTypeB) {
        return "NFC-B";
    } else if(type == FuriHalNfcTypeF) {
        return "NFC-F";
    } else if(type == FuriHalNfcTypeV) {
        return "NFC-V";
    } else {
        return "Unknown";
    }
}

const char* nfc_guess_protocol(NfcProtocol protocol) {
    if(protocol == NfcDeviceProtocolEMV) {
        return "EMV bank card";
    } else if(protocol == NfcDeviceProtocolMifareUl) {
        return "Mifare Ultral/NTAG";
    } else if(protocol == NfcDeviceProtocolMifareClassic) {
        return "Mifare Classic";
    } else if(protocol == NfcDeviceProtocolMifareDesfire) {
        return "Mifare DESFire";
    } else {
        return "Unrecognized";
    }
}

const char* nfc_mf_ul_type(MfUltralightType type, bool full_name) {
    if(type == MfUltralightTypeNTAG213) {
        return "NTAG213";
    } else if(type == MfUltralightTypeNTAG215) {
        return "NTAG215";
    } else if(type == MfUltralightTypeNTAG216) {
        return "NTAG216";
    } else if(type == MfUltralightTypeNTAGI2C1K) {
        return "NTAG I2C 1K";
    } else if(type == MfUltralightTypeNTAGI2C2K) {
        return "NTAG I2C 2K";
    } else if(type == MfUltralightTypeNTAGI2CPlus1K) {
        return "NTAG I2C Plus 1K";
    } else if(type == MfUltralightTypeNTAGI2CPlus2K) {
        return "NTAG I2C Plus 2K";
    } else if(type == MfUltralightTypeNTAG203) {
        return "NTAG203";
    } else if(type == MfUltralightTypeUL11 && full_name) {
        return "Mifare Ultralight 11";
    } else if(type == MfUltralightTypeUL21 && full_name) {
        return "Mifare Ultralight 21";
    } else {
        return "Mifare Ultralight";
    }
}

const char* nfc_mf_classic_type(MfClassicType type) {
    if(type == MfClassicTypeMini) {
        return "Mifare Mini 0.3K";
    } else if(type == MfClassicType1k) {
        return "Mifare Classic 1K";
    } else if(type == MfClassicType4k) {
        return "Mifare Classic 4K";
    } else {
        return "Mifare Classic";
    }
}

const char* nfc_felica_type(FelicaICType type) {
    if(type == FelicaICType576B) {
        return "FeliCa Classic 576B";
    } else if(type == FelicaICType2K) {
        return "FeliCa Classic 2K";
    } else if(type == FelicaICType4K) {
        return "FeliCa Classic 4K";
    } else if(type == FelicaICTypeFRAM_4K) {
        return "FeliCa Classic 4K (FRAM)";
    } else if(type == FelicaICTypeFRAM_9K) {
        return "FeliCa Classic 9K";
    } else if(type == FelicaICTypeEMV_16K) {
        return "FeliCa Classic EMV 16K";
    } else if(type == FelicaICTypeEMV_32K) {
        return "FeliCa Classic EMV 32K";
    } else if(type == FelicaICTypeEMV_36K) {
        return "FeliCa Classic EMV 36K";
    } else if(type == FelicaICTypeEMV_36K) {
        return "FeliCa Classic EMV 36K";
    } else if(type == FelicaICTypeSD1WithDES) {
        return "FeliCa SD1 (DES compatible)";
    } else if(type == FelicaICTypeSD1) {
        return "FeliCa SD1";
    } else if(type == FelicaICTypeRC_SA08) {
        return "FeliCa RC-SA08";
    } else if(type == FelicaICTypeSD2WithDES) {
        return "FeliCa SD2 (DES compatible)";
    } else if(type == FelicaICTypeSD2_4K) {
        return "FeliCa SD2 4K";
    } else if(type == FelicaICTypeSD2_6K) {
        return "FeliCa SD2 6K";
    } else if(type == FelicaICTypeRC_SA24_6K) {
        return "FeliCa RC-SA24 6K";
    } else if(type == FelicaICTypeRC_SA24_10K) {
        return "FeliCa RC-SA24 6K";
    } else if(type == FelicaICTypeMobileIC_V1) {
        return "Mobile FeliCa v1";
    } else if(type == FelicaICTypeMobileIC_V2) {
        return "Mobile FeliCa v2";
    } else if(type == FelicaICTypeMobileIC_V3) {
        return "Mobile FeliCa v3";
    } else if(type == FelicaICTypeMobileIC_V4) {
        return "Mobile FeliCa v4";
    } else if(type == FelicaICTypeMobileIC_V4_1) {
        return "Mobile FeliCa v4.1";
    } else if(type == FelicaICTypeLite) {
        return "FeliCa Lite";
    } else if(type == FelicaICTypeLiteS) {
        return "FeliCa Lite-S";
    } else if(type == FelicaICTypeLink) {
        return "FeliCa Link";
    } else if(type == FelicaICTypePlug) {
        return "FeliCa Plug";
    } else if(type == FelicaICTypeSuica) {
        return "FeliCa (SuiCa)";
    } else {
        return "FeliCa";
    }
}
