#pragma once

#include <nfc/protocols/mf_desfire/mf_desfire.h>

void nfc_mf_desfire_render_data(const MfDesfireData* data, FuriString* str);

void nfc_mf_desfire_render_version(const MfDesfireVersion* data, FuriString* str);

void nfc_mf_desfire_render_free_memory(const MfDesfireFreeMemory* data, FuriString* str);

void nfc_mf_desfire_render_key_settings(const MfDesfireKeySettings* data, FuriString* str);

void nfc_mf_desfire_render_key_version(
    const MfDesfireKeyVersion* data,
    uint32_t index,
    FuriString* str);

void nfc_mf_desfire_render_application_id(const MfDesfireApplicationId* data, FuriString* str);

void nfc_mf_desfire_render_application(const MfDesfireApplication* data, FuriString* str);

void nfc_mf_desfire_render_file_id(const MfDesfireFileId* data, FuriString* str);

void nfc_mf_desfire_render_file_settings_data(
    const MfDesfireFileSettings* settings,
    const MfDesfireFileData* data,
    FuriString* str);
