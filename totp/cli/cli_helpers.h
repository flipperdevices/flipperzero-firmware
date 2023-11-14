#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TOTP_CLI_COMMAND_NAME "totp"

#define TOTP_CLI_COLOR_ERROR "91m"
#define TOTP_CLI_COLOR_WARNING "93m"
#define TOTP_CLI_COLOR_SUCCESS "92m"
#define TOTP_CLI_COLOR_INFO "96m"

#define TOTP_CLI_PRINTF(format, ...) printf(format, ##__VA_ARGS__)

#define TOTP_CLI_PRINTF_COLORFUL(color, format, ...) \
    TOTP_CLI_PRINTF("\e[%s" format "\e[0m", color, ##__VA_ARGS__)

#define TOTP_CLI_PRINTF_ERROR(format, ...) \
    TOTP_CLI_PRINTF_COLORFUL(TOTP_CLI_COLOR_ERROR, format, ##__VA_ARGS__)
#define TOTP_CLI_PRINTF_WARNING(format, ...) \
    TOTP_CLI_PRINTF_COLORFUL(TOTP_CLI_COLOR_WARNING, format, ##__VA_ARGS__)
#define TOTP_CLI_PRINTF_SUCCESS(format, ...) \
    TOTP_CLI_PRINTF_COLORFUL(TOTP_CLI_COLOR_SUCCESS, format, ##__VA_ARGS__)
#define TOTP_CLI_PRINTF_INFO(format, ...) \
    TOTP_CLI_PRINTF_COLORFUL(TOTP_CLI_COLOR_INFO, format, ##__VA_ARGS__)

#define TOTP_CLI_LOCK_UI(plugin_state)                                  \
    Scene __previous_scene = plugin_state->current_scene;               \
    totp_scene_director_activate_scene(plugin_state, TotpSceneStandby); \
    totp_scene_director_force_redraw(plugin_state)

#define TOTP_CLI_UNLOCK_UI(plugin_state)                                \
    totp_scene_director_activate_scene(plugin_state, __previous_scene); \
    totp_scene_director_force_redraw(plugin_state)

#define TOTP_CLI_PRINT_INVALID_ARGUMENTS() \
    TOTP_CLI_PRINTF_ERROR(                 \
        "Invalid command arguments. use \"help\" command to get list of available commands")

#define TOTP_CLI_PRINT_ERROR_UPDATING_CONFIG_FILE() \
    TOTP_CLI_PRINTF_ERROR("An error has occurred during updating config file\r\n")

#define TOTP_CLI_PRINT_ERROR_LOADING_TOKEN_INFO() \
    TOTP_CLI_PRINTF_ERROR("An error has occurred during loading token information\r\n")

#define TOTP_CLI_PRINT_PROCESSING() TOTP_CLI_PRINTF("Processing, please wait...\r\n")

#define TOTP_CLI_DELETE_LAST_CHAR() \
    TOTP_CLI_PRINTF("\b \b");       \
    fflush(stdout)

#define TOTP_CLI_DELETE_CURRENT_LINE() \
    TOTP_CLI_PRINTF("\33[2K\r");       \
    fflush(stdout)

#define TOTP_CLI_DELETE_LAST_LINE()    \
    TOTP_CLI_PRINTF("\033[A\33[2K\r"); \
    fflush(stdout)

#ifdef __cplusplus
}
#endif