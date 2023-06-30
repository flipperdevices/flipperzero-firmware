#pragma once

#include "evil_portal_app.h"
#include "evil_portal_custom_event.h"
#include "evil_portal_uart.h"
#include "scenes/evil_portal_scene.h"
#include "uart_text_input.h"

#include <gui/gui.h>
#include <gui/modules/text_box.h>
#include <gui/modules/variable_item_list.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>

#define NUM_MENU_ITEMS (4)

#define EVIL_PORTAL_TEXT_BOX_STORE_SIZE (4096)
#define EVIL_PORTAL_TEXT_INPUT_STORE_SIZE (512)
#define UART_CH (FuriHalUartIdUSART1)

struct Evil_PortalApp {
  Gui *gui;
  ViewDispatcher *view_dispatcher;
  SceneManager *scene_manager;

  char* portal_logs;

  char text_input_store[EVIL_PORTAL_TEXT_INPUT_STORE_SIZE + 1];
  FuriString *text_box_store;
  size_t text_box_store_strlen;
  TextBox *text_box;
  UART_TextInput *text_input;

  VariableItemList *var_item_list;

  Evil_PortalUart *uart;
  int selected_menu_index;
  int selected_option_index[NUM_MENU_ITEMS];
  const char *selected_tx_string;
  const char *command_queue[1];
  int command_index;
  bool has_command_queue;
  bool is_command;
  bool is_custom_tx_string;
  bool focus_console_start;
  bool show_stopscan_tip;
  bool sent_ap;
  bool sent_html;
  int BAUDRATE;
  int TERMINAL_MODE; // 1=AT mode, 0=other mode

  uint8_t *index_html;
  uint8_t *ap_name;
};

typedef enum {
  Evil_PortalAppViewVarItemList,
  Evil_PortalAppViewConsoleOutput,
  Evil_PortalAppViewTextInput,
  Evil_PortalAppViewStartPortal,
} Evil_PortalAppView;
