#pragma once

#include "mf_desfire.h"

// Parse internal MfDesfire structures

void mf_desfire_version_parse(MfDesfireVersion* version, const BitBuffer* buf);

void mf_desfire_free_memory_parse(MfDesfireFreeMemory* free_mem, const BitBuffer* buf);

void mf_desfire_key_settings_parse(MfDesfireKeySettings* data, const BitBuffer* buf);

void mf_desfire_key_version_init(MfDesfireKeyVersion** data, uint32_t count);

void mf_desfire_key_version_parse(MfDesfireKeyVersion* data, const BitBuffer* buf);

// Reset internal MfDesfire structures

void mf_desfire_key_config_reset(MfDesfireKeyConfiguration* config);

void mf_desfire_file_reset(MfDesfireFile* file);

void mf_desfire_files_reset(MfDesfireFiles* files);

void mf_desfire_application_reset(MfDesfireApplication* app);

void mf_desfire_applications_reset(MfDesfireApplications* apps);

// Copy internal MfDesfire structures

void mf_desfire_key_config_copy(
    MfDesfireKeyConfiguration* config,
    const MfDesfireKeyConfiguration* other);

void mf_desfire_file_copy(MfDesfireFile* file, const MfDesfireFile* other);

void mf_desfire_files_copy(MfDesfireFiles* files, const MfDesfireFiles* other);

void mf_desfire_application_copy(MfDesfireApplication* app, const MfDesfireApplication* other);

void mf_desfire_applications_copy(MfDesfireApplications* apps, const MfDesfireApplications* other);
