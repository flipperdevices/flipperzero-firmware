#pragma once
#include "structures.h"

//Defining the UART parameters for communicating with the Raspberry Pi Zero
#define DEVICE_BAUDRATE 9600
#define LINE_DELIMITER '\n'
#define INCLUDE_LINE_DELIMITER false

//Setting the backlight on
#define BACKLIGHT_ON 1

//Creating different messages used for display
#define TAG "UHF_RFID"
#define WRITE_EPC "EPC Value"
#define WRITE_EPC_MEM "Reserved Memory"
#define WRITE_USR_MEM "User Memory"
#define WRITE_TID_MEM "TID Value"
#define WRITE_EPC_OK "EPC Written!"
#define WRITE_EPC_FAIL "Write Failed!"

//Content for the about screen
#define UHF_RFID_VERSION_APP "0.1.0"
#define UHF_RFID_MEM_DEVELOPER "@Haffnerriley"
#define UHF_RFID_GITHUB "https://github.com/haffnerriley"
#define UHF_RFID_NAME "\e#\e!       UHF RFID Reader        \e!\n"
#define UHF_RFID_BLANK_INV "\e#\e!"

//Including the different views and helper files
#include "views/view_about.h"
#include "views/view_saved.h"
#include "views/view_config.h"
#include "views/view_write.h"
#include "views/view_delete.h"
#include "views/view_epc.h"
#include "views/view_delete_success.h"
#include "views/view_epc_info.h"
#include "views/view_read.h"
#include "views/view_tag_actions.h"
#include "helpers/extract_tag_info.h"
#include "helpers/uart_process.h"
#include "helpers/saved_epc_functions.h"

//Function declarations
void main_menu_alloc(UHFReaderApp* App);

uint32_t uhf_reader_navigation_exit_callback(void* context);

void uhf_reader_submenu_callback(void* context, uint32_t index);
