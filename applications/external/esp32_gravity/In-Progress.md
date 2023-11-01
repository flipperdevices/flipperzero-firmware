* Finish restructuring menus
    * Maybe a UI for stalk
* Add selected_options_index[NUM_PURGE_ITEMS] to UART_TerminalItem
    * Change UART_TerminalApp.selected_option_index[MAX_MENU_ITEMS] to int**
    - Will no longer need purgeLoadFromMemory()
    - Enter callbacks set app->selected_option_index to scene->selected_option_index
    - Perhaps store a ref to selected_tx_string in UART_TerminalItem or a global, and only free selected_tx_string in the exit function of relevant scenes

* Re-implement console/command execution
* Re-implement cls/clear screen
* Refactor console logic - to keep console recording during menu navigation
* Flipper protocol for ESP32-Gravity to allow more sophisticated UIs
    * e.g. get all settings info, in bytes, from a single command
* Just In Time view memory allocation (now that there are lots of them)

ST25RFAL002/timer.h timerDelay(uint16_t millis)
ST25RFAL002/platform.h platformDelay(millis)
    #define platformDelay(t) furi_delay_ms(t) /*!< Performs a delay for the given time (ms)    */
furi_delay_ms(uint32_t millis)
