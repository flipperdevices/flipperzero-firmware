/**
 * @file iso7816_filesystem.h
 * @brief ISO7816-4 Virtual File System
 * 
 * Implements a hierarchical file system structure as defined in ISO/IEC 7816-4.
 * Supports MF (Master File), DF (Dedicated Files), and EF (Elementary Files).
 * 
 * Structure:
 * - MF (3F00) is the root, created by default
 * - DFs can be added as children of MF or other DFs
 * - EFs are leaf nodes containing data
 * - Applications are registered as DFs with AIDs
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Maximum length of an Application Identifier (AID) */
#define ISO7816_AID_MAX_LENGTH 16

/** Maximum number of children per DF */
#define ISO7816_MAX_CHILDREN 16

/** Maximum depth of file hierarchy */
#define ISO7816_MAX_DEPTH 8

/** Reserved file identifiers */
#define ISO7816_FID_MF       0x3F00  /**< Master File */
#define ISO7816_FID_EF_DIR   0x2F00  /**< Directory of applications */
#define ISO7816_FID_EF_ATR   0x2F01  /**< ATR/Card info */
#define ISO7816_FID_CURRENT  0x3FFF  /**< Current DF (reserved) */
#define ISO7816_FID_RESERVED 0xFFFF  /**< Reserved for future use */

/** File types as defined in ISO7816-4 */
typedef enum {
    ISO7816_FILE_TYPE_MF,                  /**< Master File (root DF) */
    ISO7816_FILE_TYPE_DF,                  /**< Dedicated File (directory) */
    ISO7816_FILE_TYPE_EF_TRANSPARENT,      /**< EF with transparent structure (binary) */
    ISO7816_FILE_TYPE_EF_LINEAR_FIXED,     /**< EF with fixed-size records */
    ISO7816_FILE_TYPE_EF_LINEAR_VARIABLE,  /**< EF with variable-size records */
    ISO7816_FILE_TYPE_EF_CYCLIC,           /**< EF with cyclic records */
} Iso7816FileType;

/** Life cycle status as defined in ISO7816-4 Table 13 */
typedef enum {
    ISO7816_LCS_NO_INFO        = 0x00,  /**< No information given */
    ISO7816_LCS_CREATION       = 0x01,  /**< Creation state */
    ISO7816_LCS_INITIALIZATION = 0x03,  /**< Initialization state */
    ISO7816_LCS_ACTIVATED      = 0x05,  /**< Operational state (activated) */
    ISO7816_LCS_DEACTIVATED    = 0x04,  /**< Operational state (deactivated) */
    ISO7816_LCS_TERMINATED     = 0x0C,  /**< Termination state */
} Iso7816LifeCycleStatus;

/** Forward declaration */
typedef struct Iso7816File Iso7816File;

/**
 * @brief Application command handler callback
 * 
 * Called when an APDU is received for a selected application.
 * 
 * @param context Application-specific context
 * @param command Incoming command APDU
 * @param response Response APDU to fill
 */
typedef void (*Iso7816AppCommandHandler)(
    void* context,
    const void* command,
    void* response);

/**
 * @brief Application lifecycle callback
 * 
 * Called when an application is selected or deselected.
 * 
 * @param context Application-specific context
 */
typedef void (*Iso7816AppLifecycleCallback)(void* context);

/** Application definition for DF with AID */
typedef struct {
    uint8_t aid[ISO7816_AID_MAX_LENGTH];  /**< Application Identifier */
    uint8_t aid_len;                       /**< Length of AID (5-16 bytes) */
    const char* name;                      /**< Human-readable name (optional) */
    Iso7816AppCommandHandler handler;      /**< Command handler callback */
    Iso7816AppLifecycleCallback on_select; /**< Called when app is selected */
    Iso7816AppLifecycleCallback on_deselect; /**< Called when app is deselected */
    void* context;                         /**< Application context */
} Iso7816AppDefinition;

/** File structure representing MF, DF, or EF */
struct Iso7816File {
    uint16_t fid;                          /**< File identifier (2 bytes) */
    uint8_t short_id;                      /**< Short EF identifier (1-30, 0=none) */
    Iso7816FileType type;                  /**< File type */
    Iso7816LifeCycleStatus lcs;            /**< Life cycle status */
    
    /** AID for DFs that are applications */
    uint8_t aid[ISO7816_AID_MAX_LENGTH];
    uint8_t aid_len;
    
    /** Application definition (NULL for non-application DFs/EFs) */
    const Iso7816AppDefinition* app;
    
    /** Data for EFs */
    uint8_t* data;
    uint16_t data_len;
    uint16_t max_size;
    
    /** Record info for record-based EFs */
    uint8_t record_size;                   /**< Fixed record size (0 for variable) */
    uint8_t record_count;                  /**< Number of records */
    
    /** Hierarchy */
    Iso7816File* parent;
    Iso7816File* children[ISO7816_MAX_CHILDREN];
    uint8_t child_count;
};

/** Maximum number of registered data objects for GET DATA */
#define ISO7816_MAX_DATA_OBJECTS 16

/** Maximum size of a single data object */
#define ISO7816_DATA_OBJECT_MAX_SIZE 128

/** Data object entry for GET DATA command */
typedef struct {
    uint16_t tag;                                  /**< Tag (P1-P2 format) */
    uint8_t data[ISO7816_DATA_OBJECT_MAX_SIZE];   /**< Data content */
    uint8_t data_len;                              /**< Length of data */
    bool active;                                   /**< Whether entry is in use */
} Iso7816DataObject;

/** Maximum PIN length */
#define ISO7816_PIN_MAX_LENGTH      16

/** Maximum number of PIN references */
#define ISO7816_MAX_PIN_REFS        8

/** PIN status */
typedef enum {
    ISO7816_PIN_STATUS_ACTIVE       = 0x00,  /**< PIN is active and can be verified */
    ISO7816_PIN_STATUS_BLOCKED      = 0x01,  /**< PIN is blocked (no retries remaining) */
    ISO7816_PIN_STATUS_NOT_INIT     = 0x02,  /**< PIN is not initialized */
} Iso7816PinStatus;

/** PIN reference data structure */
typedef struct {
    uint8_t reference;              /**< P2 value (reference qualifier) */
    uint8_t pin[ISO7816_PIN_MAX_LENGTH];  /**< PIN value */
    uint8_t pin_len;                /**< Actual PIN length */
    uint8_t retries_remaining;      /**< Remaining retry attempts */
    uint8_t max_retries;            /**< Maximum retry attempts */
    Iso7816PinStatus status;        /**< Current PIN status */
    bool verified;                  /**< Whether PIN is currently verified */
    bool active;                    /**< Whether this slot is in use */
} Iso7816PinData;

/** File system context */
typedef struct {
    Iso7816File* mf;           /**< Master File (root) */
    Iso7816File* current_df;   /**< Currently selected DF */
    Iso7816File* current_ef;   /**< Currently selected EF (if any) */
    
    /** Response buffer for GET RESPONSE */
    uint8_t* response_buffer;
    uint16_t response_len;
    uint16_t response_offset;
    
    /** Data objects for GET DATA command */
    Iso7816DataObject data_objects[ISO7816_MAX_DATA_OBJECTS];
    uint8_t data_object_count;
    
    /** PIN storage for VERIFY command */
    Iso7816PinData pins[ISO7816_MAX_PIN_REFS];
    uint8_t pin_count;
} Iso7816FileSystem;

/**
 * @brief Allocate and initialize the file system
 * 
 * Creates the MF (3F00) and standard EFs (EF.DIR, EF.ATR).
 * 
 * @return Pointer to initialized file system
 */
Iso7816FileSystem* iso7816_filesystem_alloc(void);

/**
 * @brief Free the file system and all files
 * 
 * @param fs File system to free
 */
void iso7816_filesystem_free(Iso7816FileSystem* fs);

/**
 * @brief Create a new DF under the specified parent
 * 
 * @param fs File system
 * @param parent Parent DF (NULL for MF as parent)
 * @param fid File identifier
 * @param aid Application identifier (NULL for non-application DF)
 * @param aid_len Length of AID
 * @return Created DF or NULL on error
 */
Iso7816File* iso7816_fs_create_df(
    Iso7816FileSystem* fs,
    Iso7816File* parent,
    uint16_t fid,
    const uint8_t* aid,
    uint8_t aid_len);

/**
 * @brief Create a new transparent EF under the specified parent
 * 
 * @param fs File system
 * @param parent Parent DF
 * @param fid File identifier
 * @param short_id Short EF identifier (1-30, 0 for none)
 * @param max_size Maximum data size
 * @return Created EF or NULL on error
 */
Iso7816File* iso7816_fs_create_ef_transparent(
    Iso7816FileSystem* fs,
    Iso7816File* parent,
    uint16_t fid,
    uint8_t short_id,
    uint16_t max_size);

/**
 * @brief Create a new record-based EF under the specified parent
 * 
 * @param fs File system
 * @param parent Parent DF
 * @param fid File identifier
 * @param short_id Short EF identifier (1-30, 0 for none)
 * @param type File type (LINEAR_FIXED, LINEAR_VARIABLE, or CYCLIC)
 * @param record_size Record size (0 for variable)
 * @param max_records Maximum number of records
 * @return Created EF or NULL on error
 */
Iso7816File* iso7816_fs_create_ef_record(
    Iso7816FileSystem* fs,
    Iso7816File* parent,
    uint16_t fid,
    uint8_t short_id,
    Iso7816FileType type,
    uint8_t record_size,
    uint8_t max_records);

/**
 * @brief Register an application with the file system
 * 
 * Creates a DF for the application and registers its callbacks.
 * 
 * @param fs File system
 * @param parent Parent DF (NULL for MF)
 * @param fid File identifier for the application DF
 * @param app Application definition
 * @return Application DF or NULL on error
 */
Iso7816File* iso7816_fs_register_app(
    Iso7816FileSystem* fs,
    Iso7816File* parent,
    uint16_t fid,
    const Iso7816AppDefinition* app);

/**
 * @brief Find a file by file identifier within current DF
 * 
 * @param fs File system
 * @param fid File identifier to find
 * @return Found file or NULL
 */
Iso7816File* iso7816_fs_find_by_fid(Iso7816FileSystem* fs, uint16_t fid);

/**
 * @brief Find a file by file identifier within specified DF
 * 
 * @param parent Parent DF to search in
 * @param fid File identifier to find
 * @return Found file or NULL
 */
Iso7816File* iso7816_fs_find_by_fid_in_df(Iso7816File* parent, uint16_t fid);

/**
 * @brief Find an application by AID
 * 
 * Searches the entire file system for a DF with matching AID.
 * Supports partial AID matching.
 * 
 * @param fs File system
 * @param aid Application identifier to match
 * @param aid_len Length of AID
 * @param partial If true, matches if AID starts with given bytes
 * @return Found application DF or NULL
 */
Iso7816File* iso7816_fs_find_by_aid(
    Iso7816FileSystem* fs,
    const uint8_t* aid,
    uint8_t aid_len,
    bool partial);

/**
 * @brief Find a file by short EF identifier within current DF
 * 
 * @param fs File system  
 * @param short_id Short EF identifier (1-30)
 * @return Found file or NULL
 */
Iso7816File* iso7816_fs_find_by_short_id(Iso7816FileSystem* fs, uint8_t short_id);

/**
 * @brief Select a file (DF or EF)
 * 
 * Updates current_df and/or current_ef.
 * Calls application lifecycle callbacks as needed.
 * 
 * @param fs File system
 * @param file File to select
 * @return true if selection succeeded
 */
bool iso7816_fs_select(Iso7816FileSystem* fs, Iso7816File* file);

/**
 * @brief Select parent DF of current DF
 * 
 * @param fs File system
 * @return true if selection succeeded (false if already at MF)
 */
bool iso7816_fs_select_parent(Iso7816FileSystem* fs);

/**
 * @brief Select MF (Master File)
 * 
 * @param fs File system
 * @return true if selection succeeded
 */
bool iso7816_fs_select_mf(Iso7816FileSystem* fs);

/**
 * @brief Get the currently selected application
 * 
 * @param fs File system
 * @return Currently selected application definition, or NULL
 */
const Iso7816AppDefinition* iso7816_fs_get_current_app(Iso7816FileSystem* fs);

/**
 * @brief Build FCI (File Control Information) for a file
 * 
 * Creates TLV-encoded FCI template (tag 6F) for SELECT response.
 * 
 * @param file File to describe
 * @param fci_buffer Buffer to write FCI
 * @param fci_buffer_size Size of buffer
 * @return Length of FCI data written
 */
uint16_t iso7816_fs_build_fci(
    const Iso7816File* file,
    uint8_t* fci_buffer,
    uint16_t fci_buffer_size);

/**
 * @brief Build FCP (File Control Parameters) for a file
 * 
 * Creates TLV-encoded FCP template (tag 62) for SELECT response.
 * 
 * @param file File to describe
 * @param fcp_buffer Buffer to write FCP
 * @param fcp_buffer_size Size of buffer
 * @return Length of FCP data written
 */
uint16_t iso7816_fs_build_fcp(
    const Iso7816File* file,
    uint8_t* fcp_buffer,
    uint16_t fcp_buffer_size);

/**
 * @brief Check if file is a DF (MF or DF)
 * 
 * @param file File to check
 * @return true if file is a dedicated file
 */
bool iso7816_file_is_df(const Iso7816File* file);

/**
 * @brief Check if file is an EF
 * 
 * @param file File to check
 * @return true if file is an elementary file
 */
bool iso7816_file_is_ef(const Iso7816File* file);

/**
 * @brief Write data to a transparent EF
 * 
 * @param file EF to write to
 * @param offset Offset in file
 * @param data Data to write
 * @param len Length of data
 * @return true if write succeeded
 */
bool iso7816_ef_write(Iso7816File* file, uint16_t offset, const uint8_t* data, uint16_t len);

/**
 * @brief Read data from a transparent EF
 * 
 * @param file EF to read from
 * @param offset Offset in file
 * @param buffer Buffer to read into
 * @param len Number of bytes to read
 * @return Number of bytes actually read
 */
uint16_t iso7816_ef_read(const Iso7816File* file, uint16_t offset, uint8_t* buffer, uint16_t len);

#ifdef __cplusplus
}
#endif
