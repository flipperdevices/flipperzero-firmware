#pragma once

#include <notification/notification.h>
#include <gui/gui.h>
#include <dialogs/dialogs.h>
#include "../features_config.h"
#include "../ui/totp_scenes_enum.h"
#include "../services/config/config_file_context.h"
#include "../services/idle_timeout/idle_timeout.h"
#include "notification_method.h"
#include "automation_method.h"
#ifdef TOTP_BADBT_TYPE_ENABLED
#include "../workers/bt_type_code/bt_type_code.h"
#endif
#include "../services/crypto/constants.h"

/**
 * @brief Application state structure
 */
typedef struct {
    /**
     * @brief Application current scene
     */
    Scene current_scene;

    /**
     * @brief Application current scene state
     */
    void* current_scene_state;

    /**
     * @brief Reference to the firmware dialogs subsystem 
     */
    DialogsApp* dialogs_app;

    /**
     * @brief Reference to the firmware GUI subsystem
     */
    Gui* gui;

    /**
     * @brief Timezone UTC offset in hours 
     */
    float timezone_offset;

    /**
     * @brief Config file context
     */
    ConfigFileContext* config_file_context;

    /**
     * @brief Encrypted well-known data
     */
    uint8_t* crypto_verify_data;

    /**
     * @brief Encrypted well-known data length
     */
    size_t crypto_verify_data_length;

    /**
     * @brief Whether PIN is set by user or not 
     */
    bool pin_set;

    /**
     * @brief Initialization vector (IV) to be used for encryption\decryption 
     */
    uint8_t iv[CRYPTO_IV_LENGTH];

    /**
     * @brief Basic randomly-generated initialization vector (IV)
     */
    uint8_t base_iv[CRYPTO_IV_LENGTH];

    /**
     * @brief Notification method
     */
    NotificationMethod notification_method;

    /**
     * @brief Automation method
     */
    AutomationMethod automation_method;

#ifdef TOTP_BADBT_TYPE_ENABLED
    /**
     * @brief Bad-Bluetooth worker context
     */
    TotpBtTypeCodeWorkerContext* bt_type_code_worker_context;
#endif

    /**
     * @brief IDLE timeout context
     */
    IdleTimeoutContext* idle_timeout_context;

    /**
     * @brief Font index to be used to draw TOTP token
     */
    uint8_t active_font_index;

    /**
     * @brief Crypto key slot to be used
     */
    uint8_t crypto_key_slot;

    /**
     * @brief Crypto algorithms version to be used
     */
    uint8_t crypto_version;

    /**
     * @brief Application even queue
     */
    FuriMessageQueue* event_queue;
} PluginState;
