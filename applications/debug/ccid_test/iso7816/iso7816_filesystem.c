/**
 * @file iso7816_filesystem.c
 * @brief ISO7816-4 Virtual File System Implementation
 */

#include "iso7816_filesystem.h"
#include <furi.h>
#include <stdlib.h>
#include <string.h>

#define TAG "ISO7816_FS"

/** Response buffer size for GET RESPONSE */
#define RESPONSE_BUFFER_SIZE 256

/**
 * @brief Allocate and initialize a file structure
 */
static Iso7816File* iso7816_file_alloc(void) {
    Iso7816File* file = malloc(sizeof(Iso7816File));
    if(file) {
        memset(file, 0, sizeof(Iso7816File));
        file->lcs = ISO7816_LCS_ACTIVATED;
    }
    return file;
}

/**
 * @brief Free a file and all its children recursively
 */
static void iso7816_file_free(Iso7816File* file) {
    if(!file) return;
    
    // Free all children first
    for(uint8_t i = 0; i < file->child_count; i++) {
        iso7816_file_free(file->children[i]);
    }
    
    // Free data buffer for EFs
    if(file->data) {
        free(file->data);
    }
    
    free(file);
}

/**
 * @brief Add a child file to a DF
 */
static bool iso7816_df_add_child(Iso7816File* parent, Iso7816File* child) {
    furi_assert(parent);
    furi_assert(child);
    furi_assert(iso7816_file_is_df(parent));
    
    if(parent->child_count >= ISO7816_MAX_CHILDREN) {
        FURI_LOG_E(TAG, "Maximum children reached for DF %04X", parent->fid);
        return false;
    }
    
    parent->children[parent->child_count++] = child;
    child->parent = parent;
    return true;
}

/**
 * @brief Search for a file by AID recursively
 */
static Iso7816File* iso7816_fs_find_by_aid_recursive(
    Iso7816File* df,
    const uint8_t* aid,
    uint8_t aid_len,
    bool partial) {
    
    if(!df || !aid || aid_len == 0) return NULL;
    
    // Check this DF
    if(df->aid_len > 0) {
        if(partial) {
            // Partial match: AID starts with given bytes
            if(df->aid_len >= aid_len && memcmp(df->aid, aid, aid_len) == 0) {
                return df;
            }
        } else {
            // Exact match
            if(df->aid_len == aid_len && memcmp(df->aid, aid, aid_len) == 0) {
                return df;
            }
        }
    }
    
    // Search children
    for(uint8_t i = 0; i < df->child_count; i++) {
        Iso7816File* child = df->children[i];
        if(iso7816_file_is_df(child)) {
            Iso7816File* found = iso7816_fs_find_by_aid_recursive(child, aid, aid_len, partial);
            if(found) return found;
        }
    }
    
    return NULL;
}

Iso7816FileSystem* iso7816_filesystem_alloc(void) {
    Iso7816FileSystem* fs = malloc(sizeof(Iso7816FileSystem));
    if(!fs) return NULL;
    
    memset(fs, 0, sizeof(Iso7816FileSystem));
    
    // Create MF (Master File)
    fs->mf = iso7816_file_alloc();
    if(!fs->mf) {
        free(fs);
        return NULL;
    }
    
    fs->mf->fid = ISO7816_FID_MF;
    fs->mf->type = ISO7816_FILE_TYPE_MF;
    fs->mf->lcs = ISO7816_LCS_ACTIVATED;
    
    // Start with MF selected
    fs->current_df = fs->mf;
    fs->current_ef = NULL;
    
    // Allocate response buffer
    fs->response_buffer = malloc(RESPONSE_BUFFER_SIZE);
    fs->response_len = 0;
    fs->response_offset = 0;
    
    // Create standard EFs under MF
    // EF.DIR (2F00) - Directory of applications
    Iso7816File* ef_dir = iso7816_fs_create_ef_transparent(fs, fs->mf, ISO7816_FID_EF_DIR, 0, 256);
    if(ef_dir) {
        ef_dir->short_id = 30; // Common short ID for EF.DIR
    }
    
    // EF.ATR (2F01) - ATR/card info  
    Iso7816File* ef_atr = iso7816_fs_create_ef_transparent(fs, fs->mf, ISO7816_FID_EF_ATR, 0, 64);
    if(ef_atr) {
        ef_atr->short_id = 29; // Common short ID for EF.ATR
    }
    
    FURI_LOG_I(TAG, "File system initialized with MF (3F00)");
    
    return fs;
}

void iso7816_filesystem_free(Iso7816FileSystem* fs) {
    if(!fs) return;
    
    // Free the file tree starting from MF
    iso7816_file_free(fs->mf);
    
    // Free response buffer
    if(fs->response_buffer) {
        free(fs->response_buffer);
    }
    
    free(fs);
}

Iso7816File* iso7816_fs_create_df(
    Iso7816FileSystem* fs,
    Iso7816File* parent,
    uint16_t fid,
    const uint8_t* aid,
    uint8_t aid_len) {
    
    furi_assert(fs);
    
    if(!parent) {
        parent = fs->mf;
    }
    
    if(!iso7816_file_is_df(parent)) {
        FURI_LOG_E(TAG, "Parent is not a DF");
        return NULL;
    }
    
    // Check if FID already exists
    if(iso7816_fs_find_by_fid_in_df(parent, fid)) {
        FURI_LOG_E(TAG, "File %04X already exists", fid);
        return NULL;
    }
    
    Iso7816File* df = iso7816_file_alloc();
    if(!df) return NULL;
    
    df->fid = fid;
    df->type = ISO7816_FILE_TYPE_DF;
    
    // Copy AID if provided
    if(aid && aid_len > 0 && aid_len <= ISO7816_AID_MAX_LENGTH) {
        memcpy(df->aid, aid, aid_len);
        df->aid_len = aid_len;
    }
    
    if(!iso7816_df_add_child(parent, df)) {
        free(df);
        return NULL;
    }
    
    FURI_LOG_I(TAG, "Created DF %04X under %04X", fid, parent->fid);
    return df;
}

Iso7816File* iso7816_fs_create_ef_transparent(
    Iso7816FileSystem* fs,
    Iso7816File* parent,
    uint16_t fid,
    uint8_t short_id,
    uint16_t max_size) {
    
    furi_assert(fs);
    furi_assert(parent);
    
    if(!iso7816_file_is_df(parent)) {
        FURI_LOG_E(TAG, "Parent is not a DF");
        return NULL;
    }
    
    // Check if FID already exists
    if(iso7816_fs_find_by_fid_in_df(parent, fid)) {
        FURI_LOG_E(TAG, "File %04X already exists", fid);
        return NULL;
    }
    
    // Validate short ID
    if(short_id > 30) {
        FURI_LOG_E(TAG, "Invalid short EF ID: %d", short_id);
        return NULL;
    }
    
    Iso7816File* ef = iso7816_file_alloc();
    if(!ef) return NULL;
    
    ef->fid = fid;
    ef->type = ISO7816_FILE_TYPE_EF_TRANSPARENT;
    ef->short_id = short_id;
    ef->max_size = max_size;
    
    // Allocate data buffer
    if(max_size > 0) {
        ef->data = malloc(max_size);
        if(!ef->data) {
            free(ef);
            return NULL;
        }
        memset(ef->data, 0, max_size);
    }
    
    if(!iso7816_df_add_child(parent, ef)) {
        if(ef->data) free(ef->data);
        free(ef);
        return NULL;
    }
    
    FURI_LOG_D(TAG, "Created transparent EF %04X under %04X (max %d bytes)", 
               fid, parent->fid, max_size);
    return ef;
}

Iso7816File* iso7816_fs_create_ef_record(
    Iso7816FileSystem* fs,
    Iso7816File* parent,
    uint16_t fid,
    uint8_t short_id,
    Iso7816FileType type,
    uint8_t record_size,
    uint8_t max_records) {
    
    furi_assert(fs);
    furi_assert(parent);
    
    // Validate type
    if(type != ISO7816_FILE_TYPE_EF_LINEAR_FIXED &&
       type != ISO7816_FILE_TYPE_EF_LINEAR_VARIABLE &&
       type != ISO7816_FILE_TYPE_EF_CYCLIC) {
        FURI_LOG_E(TAG, "Invalid record EF type");
        return NULL;
    }
    
    if(!iso7816_file_is_df(parent)) {
        FURI_LOG_E(TAG, "Parent is not a DF");
        return NULL;
    }
    
    if(iso7816_fs_find_by_fid_in_df(parent, fid)) {
        FURI_LOG_E(TAG, "File %04X already exists", fid);
        return NULL;
    }
    
    if(short_id > 30) {
        FURI_LOG_E(TAG, "Invalid short EF ID: %d", short_id);
        return NULL;
    }
    
    Iso7816File* ef = iso7816_file_alloc();
    if(!ef) return NULL;
    
    ef->fid = fid;
    ef->type = type;
    ef->short_id = short_id;
    ef->record_size = record_size;
    ef->record_count = 0;
    
    // For fixed records, allocate full buffer
    if(type == ISO7816_FILE_TYPE_EF_LINEAR_FIXED || type == ISO7816_FILE_TYPE_EF_CYCLIC) {
        ef->max_size = (uint16_t)record_size * max_records;
        if(ef->max_size > 0) {
            ef->data = malloc(ef->max_size);
            if(!ef->data) {
                free(ef);
                return NULL;
            }
            memset(ef->data, 0, ef->max_size);
        }
    }
    
    if(!iso7816_df_add_child(parent, ef)) {
        if(ef->data) free(ef->data);
        free(ef);
        return NULL;
    }
    
    FURI_LOG_D(TAG, "Created record EF %04X under %04X", fid, parent->fid);
    return ef;
}

Iso7816File* iso7816_fs_register_app(
    Iso7816FileSystem* fs,
    Iso7816File* parent,
    uint16_t fid,
    const Iso7816AppDefinition* app) {
    
    furi_assert(fs);
    furi_assert(app);
    furi_assert(app->aid_len >= 5 && app->aid_len <= 16);
    
    // Create DF for the application
    Iso7816File* df = iso7816_fs_create_df(fs, parent, fid, app->aid, app->aid_len);
    if(!df) return NULL;
    
    // Register application callbacks
    df->app = app;
    
    FURI_LOG_I(TAG, "Registered application '%s' with AID length %d", 
               app->name ? app->name : "unnamed", app->aid_len);
    
    return df;
}

Iso7816File* iso7816_fs_find_by_fid(Iso7816FileSystem* fs, uint16_t fid) {
    furi_assert(fs);
    
    // Handle special FIDs
    if(fid == ISO7816_FID_MF) {
        return fs->mf;
    }
    
    // Search in current DF
    return iso7816_fs_find_by_fid_in_df(fs->current_df, fid);
}

Iso7816File* iso7816_fs_find_by_fid_in_df(Iso7816File* parent, uint16_t fid) {
    if(!parent) return NULL;
    
    // Check if it's the parent itself
    if(parent->fid == fid) {
        return parent;
    }
    
    // Search children
    for(uint8_t i = 0; i < parent->child_count; i++) {
        if(parent->children[i]->fid == fid) {
            return parent->children[i];
        }
    }
    
    return NULL;
}

Iso7816File* iso7816_fs_find_by_aid(
    Iso7816FileSystem* fs,
    const uint8_t* aid,
    uint8_t aid_len,
    bool partial) {
    
    furi_assert(fs);
    return iso7816_fs_find_by_aid_recursive(fs->mf, aid, aid_len, partial);
}

Iso7816File* iso7816_fs_find_by_short_id(Iso7816FileSystem* fs, uint8_t short_id) {
    furi_assert(fs);
    
    if(short_id == 0 || short_id > 30) return NULL;
    
    Iso7816File* df = fs->current_df;
    for(uint8_t i = 0; i < df->child_count; i++) {
        if(df->children[i]->short_id == short_id) {
            return df->children[i];
        }
    }
    
    return NULL;
}

bool iso7816_fs_select(Iso7816FileSystem* fs, Iso7816File* file) {
    furi_assert(fs);
    
    if(!file) return false;
    
    // Check if file is activated
    if(file->lcs != ISO7816_LCS_ACTIVATED) {
        FURI_LOG_W(TAG, "Cannot select deactivated file %04X", file->fid);
        return false;
    }
    
    // If selecting a DF
    if(iso7816_file_is_df(file)) {
        // Call deselect on current app if different
        if(fs->current_df && fs->current_df->app && fs->current_df != file) {
            if(fs->current_df->app->on_deselect) {
                fs->current_df->app->on_deselect(fs->current_df->app->context);
            }
        }
        
        fs->current_df = file;
        fs->current_ef = NULL;
        
        // Call select on new app
        if(file->app && file->app->on_select) {
            file->app->on_select(file->app->context);
        }
        
        FURI_LOG_D(TAG, "Selected DF %04X", file->fid);
    } else {
        // Selecting an EF
        fs->current_ef = file;
        FURI_LOG_D(TAG, "Selected EF %04X", file->fid);
    }
    
    return true;
}

bool iso7816_fs_select_parent(Iso7816FileSystem* fs) {
    furi_assert(fs);
    
    if(!fs->current_df || !fs->current_df->parent) {
        // Already at MF or no parent
        return false;
    }
    
    return iso7816_fs_select(fs, fs->current_df->parent);
}

bool iso7816_fs_select_mf(Iso7816FileSystem* fs) {
    furi_assert(fs);
    return iso7816_fs_select(fs, fs->mf);
}

const Iso7816AppDefinition* iso7816_fs_get_current_app(Iso7816FileSystem* fs) {
    furi_assert(fs);
    
    // Walk up the hierarchy to find an app
    Iso7816File* df = fs->current_df;
    while(df) {
        if(df->app) {
            return df->app;
        }
        df = df->parent;
    }
    
    return NULL;
}

bool iso7816_file_is_df(const Iso7816File* file) {
    if(!file) return false;
    return (file->type == ISO7816_FILE_TYPE_MF || file->type == ISO7816_FILE_TYPE_DF);
}

bool iso7816_file_is_ef(const Iso7816File* file) {
    if(!file) return false;
    return (file->type == ISO7816_FILE_TYPE_EF_TRANSPARENT ||
            file->type == ISO7816_FILE_TYPE_EF_LINEAR_FIXED ||
            file->type == ISO7816_FILE_TYPE_EF_LINEAR_VARIABLE ||
            file->type == ISO7816_FILE_TYPE_EF_CYCLIC);
}

uint16_t iso7816_fs_build_fci(
    const Iso7816File* file,
    uint8_t* fci_buffer,
    uint16_t fci_buffer_size) {
    
    if(!file || !fci_buffer || fci_buffer_size < 4) return 0;
    
    uint16_t pos = 0;
    uint16_t content_start;
    
    // FCI template tag (6F)
    fci_buffer[pos++] = 0x6F;
    // Length placeholder (will fill in later)
    content_start = pos;
    pos++; // Skip length byte for now
    
    // File identifier (83)
    fci_buffer[pos++] = 0x83;
    fci_buffer[pos++] = 0x02;
    fci_buffer[pos++] = (file->fid >> 8) & 0xFF;
    fci_buffer[pos++] = file->fid & 0xFF;
    
    // DF name / AID (84) - only for DFs with AID
    if(iso7816_file_is_df(file) && file->aid_len > 0) {
        if(pos + 2 + file->aid_len <= fci_buffer_size) {
            fci_buffer[pos++] = 0x84;
            fci_buffer[pos++] = file->aid_len;
            memcpy(&fci_buffer[pos], file->aid, file->aid_len);
            pos += file->aid_len;
        }
    }
    
    // File descriptor byte (82)
    fci_buffer[pos++] = 0x82;
    fci_buffer[pos++] = 0x01;
    switch(file->type) {
        case ISO7816_FILE_TYPE_MF:
        case ISO7816_FILE_TYPE_DF:
            fci_buffer[pos++] = 0x38; // DF
            break;
        case ISO7816_FILE_TYPE_EF_TRANSPARENT:
            fci_buffer[pos++] = 0x01; // Transparent
            break;
        case ISO7816_FILE_TYPE_EF_LINEAR_FIXED:
            fci_buffer[pos++] = 0x02; // Linear fixed
            break;
        case ISO7816_FILE_TYPE_EF_LINEAR_VARIABLE:
            fci_buffer[pos++] = 0x04; // Linear variable
            break;
        case ISO7816_FILE_TYPE_EF_CYCLIC:
            fci_buffer[pos++] = 0x06; // Cyclic
            break;
        default:
            fci_buffer[pos++] = 0x00;
            break;
    }
    
    // Life cycle status (8A)
    fci_buffer[pos++] = 0x8A;
    fci_buffer[pos++] = 0x01;
    fci_buffer[pos++] = (uint8_t)file->lcs;
    
    // For EFs, add size info (80)
    if(iso7816_file_is_ef(file)) {
        fci_buffer[pos++] = 0x80;
        fci_buffer[pos++] = 0x02;
        fci_buffer[pos++] = (file->data_len >> 8) & 0xFF;
        fci_buffer[pos++] = file->data_len & 0xFF;
    }
    
    // Fill in length
    fci_buffer[content_start] = pos - content_start - 1;
    
    return pos;
}

uint16_t iso7816_fs_build_fcp(
    const Iso7816File* file,
    uint8_t* fcp_buffer,
    uint16_t fcp_buffer_size) {
    
    if(!file || !fcp_buffer || fcp_buffer_size < 4) return 0;
    
    uint16_t pos = 0;
    uint16_t content_start;
    
    // FCP template tag (62)
    fcp_buffer[pos++] = 0x62;
    content_start = pos;
    pos++; // Skip length byte for now
    
    // File descriptor byte (82)
    fcp_buffer[pos++] = 0x82;
    fcp_buffer[pos++] = 0x01;
    switch(file->type) {
        case ISO7816_FILE_TYPE_MF:
        case ISO7816_FILE_TYPE_DF:
            fcp_buffer[pos++] = 0x38;
            break;
        case ISO7816_FILE_TYPE_EF_TRANSPARENT:
            fcp_buffer[pos++] = 0x01;
            break;
        case ISO7816_FILE_TYPE_EF_LINEAR_FIXED:
            fcp_buffer[pos++] = 0x02;
            break;
        case ISO7816_FILE_TYPE_EF_LINEAR_VARIABLE:
            fcp_buffer[pos++] = 0x04;
            break;
        case ISO7816_FILE_TYPE_EF_CYCLIC:
            fcp_buffer[pos++] = 0x06;
            break;
        default:
            fcp_buffer[pos++] = 0x00;
            break;
    }
    
    // File identifier (83)
    fcp_buffer[pos++] = 0x83;
    fcp_buffer[pos++] = 0x02;
    fcp_buffer[pos++] = (file->fid >> 8) & 0xFF;
    fcp_buffer[pos++] = file->fid & 0xFF;
    
    // DF name (84)
    if(iso7816_file_is_df(file) && file->aid_len > 0) {
        if(pos + 2 + file->aid_len <= fcp_buffer_size) {
            fcp_buffer[pos++] = 0x84;
            fcp_buffer[pos++] = file->aid_len;
            memcpy(&fcp_buffer[pos], file->aid, file->aid_len);
            pos += file->aid_len;
        }
    }
    
    // Life cycle status (8A)
    fcp_buffer[pos++] = 0x8A;
    fcp_buffer[pos++] = 0x01;
    fcp_buffer[pos++] = (uint8_t)file->lcs;
    
    // Fill in length
    fcp_buffer[content_start] = pos - content_start - 1;
    
    return pos;
}

bool iso7816_ef_write(Iso7816File* file, uint16_t offset, const uint8_t* data, uint16_t len) {
    if(!file || !data || !iso7816_file_is_ef(file)) return false;
    if(file->type != ISO7816_FILE_TYPE_EF_TRANSPARENT) return false;
    if(!file->data) return false;
    
    if(offset + len > file->max_size) {
        FURI_LOG_E(TAG, "Write exceeds file size");
        return false;
    }
    
    memcpy(file->data + offset, data, len);
    
    // Update data length if we wrote past current end
    if(offset + len > file->data_len) {
        file->data_len = offset + len;
    }
    
    return true;
}

uint16_t iso7816_ef_read(const Iso7816File* file, uint16_t offset, uint8_t* buffer, uint16_t len) {
    if(!file || !buffer || !iso7816_file_is_ef(file)) return 0;
    if(file->type != ISO7816_FILE_TYPE_EF_TRANSPARENT) return 0;
    if(!file->data) return 0;
    
    if(offset >= file->data_len) {
        return 0;
    }
    
    uint16_t available = file->data_len - offset;
    uint16_t to_read = (len < available) ? len : available;
    
    memcpy(buffer, file->data + offset, to_read);
    return to_read;
}
