#pragma once

#include "mf_desfire.h"

// SimpleArray configurations

extern const SimpleArrayConfig mf_desfire_key_version_array_config;
extern const SimpleArrayConfig mf_desfire_app_id_array_config;
extern const SimpleArrayConfig mf_desfire_app_array_config;

// Parse internal MfDesfire structures

void mf_desfire_version_parse(MfDesfireVersion* data, const BitBuffer* buf);

void mf_desfire_free_memory_parse(MfDesfireFreeMemory* data, const BitBuffer* buf);

void mf_desfire_key_settings_parse(MfDesfireKeySettings* data, const BitBuffer* buf);

void mf_desfire_key_version_parse(MfDesfireKeyVersion* data, const BitBuffer* buf);

void mf_desfire_application_id_parse(
    MfDesfireApplicationId data,
    uint32_t index,
    const BitBuffer* buf);

// Reset internal MfDesfire structures

void mf_desfire_file_reset(MfDesfireFile* data);

void mf_desfire_files_reset(MfDesfireFiles* data);

void mf_desfire_application_reset(MfDesfireApplication* data);

// Copy internal MfDesfire structures

// void mf_desfire_key_config_copy(
//     MfDesfireKeyConfiguration* data,
//     const MfDesfireKeyConfiguration* other);

// void mf_desfire_file_copy(MfDesfireFile* data, const MfDesfireFile* other);
//
// void mf_desfire_files_copy(MfDesfireFiles* data, const MfDesfireFiles* other);
//
// void mf_desfire_application_copy(MfDesfireApplication* data, const MfDesfireApplication* other);
//
// void mf_desfire_applications_copy(MfDesfireApplications* data, const MfDesfireApplications* other);
