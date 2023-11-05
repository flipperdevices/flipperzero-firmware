* Finish restructuring menus
    * Maybe a UI for stalk
* header text in text input box is full of overlapping text, even after slimming help text right down. Why???

* Settings->MAC doesn't need a get option, just launch the view to get or set
* Options that display their value instead of 'get' don't need multiple options - just select it to set
* Re-implement console/command execution
* Re-implement cls/clear screen
* Refactor console logic - to keep console recording during menu navigation
* Flipper protocol for ESP32-Gravity to allow more sophisticated UIs
    * e.g. get all settings info, in bytes, from a single command
* Just In Time view memory allocation (now that there are lots of them)

esp32 command: sync
Return all settings in a fixed, parseable format. (<syncItem>:<value>)+

ST25RFAL002/timer.h timerDelay(uint16_t millis)
ST25RFAL002/platform.h platformDelay(millis)
    #define platformDelay(t) furi_delay_ms(t) /*!< Performs a delay for the given time (ms)    */
furi_delay_ms(uint32_t millis)
