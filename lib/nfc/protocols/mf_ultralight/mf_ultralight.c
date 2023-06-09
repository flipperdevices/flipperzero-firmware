#include "mf_ultralight.h"

#include <furi.h>

typedef struct {
    uint16_t total_pages;
    uint16_t config_page;
    uint32_t feature_set;
} MfUltralightFeatures;

static const MfUltralightFeatures mf_ultralight_features[MfUltralightTypeNum] = {
    [MfUltralightTypeUnknown] =
        {
            .total_pages = 16,
            .config_page = 0,
            .feature_set = MfUltralightFeatureSupportCompatibleWrite,
        },
    [MfUltralightTypeNTAG203] =
        {
            .total_pages = 42,
            .config_page = 0,
            .feature_set = MfUltralightFeatureSupportCompatibleWrite |
                           MfUltralightFeatureSupportCounterInMemory,
        },
    [MfUltralightTypeUL11] =
        {
            .total_pages = 20,
            .config_page = 16,
            .feature_set =
                MfUltralightFeatureSupportReadVersion | MfUltralightFeatureSupportReadSignature |
                MfUltralightFeatureSupportReadCounter |
                MfUltralightFeatureSupportCheckTearingFlag | MfUltralightFeatureSupportFastRead |
                MfUltralightFeatureSupportIncCounter | MfUltralightFeatureSupportCompatibleWrite |
                MfUltralightFeatureSupportAuthentication | MfUltralightFeatureSupportVcsl,
        },
    [MfUltralightTypeUL21] =
        {
            .total_pages = 41,
            .config_page = 37,
            .feature_set =
                MfUltralightFeatureSupportReadVersion | MfUltralightFeatureSupportReadSignature |
                MfUltralightFeatureSupportReadCounter |
                MfUltralightFeatureSupportCheckTearingFlag | MfUltralightFeatureSupportFastRead |
                MfUltralightFeatureSupportIncCounter | MfUltralightFeatureSupportCompatibleWrite |
                MfUltralightFeatureSupportAuthentication | MfUltralightFeatureSupportVcsl,
        },
    [MfUltralightTypeNTAG213] =
        {
            .total_pages = 45,
            .config_page = 41,
            .feature_set =
                MfUltralightFeatureSupportReadVersion | MfUltralightFeatureSupportReadSignature |
                MfUltralightFeatureSupportReadCounter | MfUltralightFeatureSupportFastRead |
                MfUltralightFeatureSupportCompatibleWrite |
                MfUltralightFeatureSupportAuthentication |
                MfUltralightFeatureSupportSingleCounter | MfUltralightFeatureSupportAsciiMirror,
        },
    [MfUltralightTypeNTAG215] =
        {
            .total_pages = 135,
            .config_page = 131,
            .feature_set =
                MfUltralightFeatureSupportReadVersion | MfUltralightFeatureSupportReadSignature |
                MfUltralightFeatureSupportReadCounter | MfUltralightFeatureSupportFastRead |
                MfUltralightFeatureSupportCompatibleWrite |
                MfUltralightFeatureSupportAuthentication |
                MfUltralightFeatureSupportSingleCounter | MfUltralightFeatureSupportAsciiMirror,
        },
    [MfUltralightTypeNTAG216] =
        {
            .total_pages = 231,
            .config_page = 227,
            .feature_set =
                MfUltralightFeatureSupportReadVersion | MfUltralightFeatureSupportReadSignature |
                MfUltralightFeatureSupportReadCounter | MfUltralightFeatureSupportFastRead |
                MfUltralightFeatureSupportCompatibleWrite |
                MfUltralightFeatureSupportAuthentication |
                MfUltralightFeatureSupportSingleCounter | MfUltralightFeatureSupportAsciiMirror,
        },
    [MfUltralightTypeNTAGI2C1K] =
        {
            .total_pages = 231,
            .config_page = 0,
            .feature_set =
                MfUltralightFeatureSupportReadVersion | MfUltralightFeatureSupportFastRead |
                MfUltralightFeatureSupportFastWrite | MfUltralightFeatureSupportSectorSelect,
        },
    [MfUltralightTypeNTAGI2C2K] =
        {
            .total_pages = 485,
            .config_page = 0,
            .feature_set =
                MfUltralightFeatureSupportReadVersion | MfUltralightFeatureSupportFastRead |
                MfUltralightFeatureSupportFastWrite | MfUltralightFeatureSupportSectorSelect,
        },
    [MfUltralightTypeNTAGI2CPlus1K] =
        {
            .total_pages = 236,
            .config_page = 227,
            .feature_set =
                MfUltralightFeatureSupportReadVersion | MfUltralightFeatureSupportReadSignature |
                MfUltralightFeatureSupportFastRead | MfUltralightFeatureSupportAuthentication |
                MfUltralightFeatureSupportSectorSelect,
        },
    [MfUltralightTypeNTAGI2CPlus2K] =
        {
            .total_pages = 492,
            .config_page = 227,
            .feature_set =
                MfUltralightFeatureSupportReadVersion | MfUltralightFeatureSupportReadSignature |
                MfUltralightFeatureSupportFastRead | MfUltralightFeatureSupportAuthentication |
                MfUltralightFeatureSupportSectorSelect,
        },
};

MfUltralightData* mf_ultralight_alloc() {
    MfUltralightData* data = malloc(sizeof(MfUltralightData));
    data->nfca_data = nfca_alloc();
    return data;
}

void mf_ultralight_free(MfUltralightData* data) {
    furi_assert(data);

    nfca_free(data->nfca_data);
    free(data);
}

void mf_ultralight_reset(MfUltralightData* data) {
    furi_assert(data);

    nfca_reset(data->nfca_data);
}

void mf_ultralight_copy(MfUltralightData* data, const MfUltralightData* other) {
    furi_assert(data);
    furi_assert(other);

    nfca_copy(data->nfca_data, other->nfca_data);
    memcpy(data->counter, other->counter, MF_ULTRALIGHT_COUNTER_NUM);
    memcpy(data->tearing_flag, other->tearing_flag, MF_ULTRALIGHT_TEARING_FLAG_NUM);
    memcpy(data->page, other->page, MF_ULTRALIGHT_MAX_PAGE_NUM);

    data->type = other->type;
    data->version = other->version;
    data->signature = other->signature;

    data->pages_read = other->pages_read;
    data->pages_total = other->pages_total;
    data->auth_attempts = other->auth_attempts;
}

MfUltralightType mf_ultralight_get_type_by_version(MfUltralightVersion* version) {
    furi_assert(version);

    MfUltralightType type = MfUltralightTypeUnknown;

    if(version->storage_size == 0x0B || version->storage_size == 0x00) {
        type = MfUltralightTypeUL11;
    } else if(version->storage_size == 0x0E) {
        type = MfUltralightTypeUL21;
    } else if(version->storage_size == 0x0F) {
        type = MfUltralightTypeNTAG213;
    } else if(version->storage_size == 0x11) {
        type = MfUltralightTypeNTAG215;
    } else if(version->prod_subtype == 5 && version->prod_ver_major == 2) {
        if(version->prod_ver_minor == 1) {
            if(version->storage_size == 0x13) {
                type = MfUltralightTypeNTAGI2C1K;
            } else if(version->storage_size == 0x15) {
                type = MfUltralightTypeNTAGI2C2K;
            }
        } else if(version->prod_ver_minor == 2) {
            if(version->storage_size == 0x13) {
                type = MfUltralightTypeNTAGI2CPlus1K;
            } else if(version->storage_size == 0x15) {
                type = MfUltralightTypeNTAGI2CPlus2K;
            }
        }
    } else if(version->storage_size == 0x13) {
        type = MfUltralightTypeNTAG216;
    }

    return type;
}

uint16_t mf_ultralight_get_pages_total(MfUltralightType type) {
    return mf_ultralight_features[type].total_pages;
}

uint32_t mf_ultralight_get_feature_support_set(MfUltralightType type) {
    return mf_ultralight_features[type].feature_set;
}

const char* mf_ultralight_get_name(MfUltralightType type, bool full_name) {
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

bool mf_ultralight_detect_protocol(NfcaData* nfca_data) {
    furi_assert(nfca_data);

    bool mfu_detected = (nfca_data->atqa[0] == 0x44) && (nfca_data->atqa[1] == 0x00) &&
                        (nfca_data->sak == 0x00);

    return mfu_detected;
}

uint16_t mf_ultralight_get_config_page_num(MfUltralightType type) {
    return mf_ultralight_features[type].config_page;
}

bool mf_ultralight_get_config_page(MfUltralightData* data, MfUltralightConfigPages** config) {
    furi_assert(data);
    furi_assert(config);

    bool config_pages_found = false;

    uint16_t config_page = mf_ultralight_features[data->type].config_page;
    if(config_page != 0) {
        *config = (MfUltralightConfigPages*)&data->page[config_page];
        config_pages_found = true;
    }

    return config_pages_found;
}

bool mf_ultralight_is_all_data_read(MfUltralightData* data) {
    furi_assert(data);

    bool all_read = false;
    if(data->pages_read == data->pages_total) {
        // Having read all the pages doesn't mean that we've got everything.
        // By default PWD is 0xFFFFFFFF, but if read back it is always 0x00000000,
        // so a default read on an auth-supported NTAG is never complete.
        uint32_t feature_set = mf_ultralight_get_feature_support_set(data->type);
        if(feature_set & MfUltralightFeatureSupportAuthentication) {
            all_read = true;
        } else {
            MfUltralightConfigPages* config = NULL;
            if(mf_ultralight_get_config_page(data, &config)) {
                all_read = ((config->password.pass != 0) || (config->pack.pack != 0));
            }
        }
    }

    return all_read;
}

bool mf_ultralight_is_counter_configured(MfUltralightData* data) {
    furi_assert(data);

    MfUltralightConfigPages* config = NULL;
    bool configured = false;

    switch(data->type) {
    case MfUltralightTypeNTAG213:
    case MfUltralightTypeNTAG215:
    case MfUltralightTypeNTAG216:
        if(mf_ultralight_get_config_page(data, &config)) {
            configured = config->access.nfc_cnt_en;
        }
        break;

    default:
        configured = true;
        break;
    }

    return configured;
}
