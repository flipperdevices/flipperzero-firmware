* Finish restructuring menus
    * Maybe a UI for stalk
* header text in text input box is full of overlapping text, even after slimming help text right down. Why???
* Ability to save and load settings to/from Flipper (no choice of filename)
* Ability to save and load data to/from flipper (no choice of filename)
* Options that display their value instead of 'get' don't need multiple options - just select it to set
* When saving settings display directory contents along with 'New File...', allowing you to overwrite existing file or create new (.settings extension)
* When loading settings display directory contents, allowing any file to be selected
* When saving data display directory contents along with 'New File...', allowing you to overwrite existing file or create new (.gravity extension)
* When loading data display directory contents, allowing any file to be selected
* Refactor console logic - to keep console recording during menu navigation
* Replace view, select and selected UIs with a menu list (menu list, not variable item list)
  * A single UI for the 3 features
  * View (choice between AP, STA, BT, BT Services, Selected AP, Selected STA, Selected BT, Selected Services [can I select services?])
  * Selected items are indicated on the UI. Perhaps hide indicator when viewing selected items
  * Select implemented with OK button
* Just In Time view memory allocation (now that there are lots of them)

esp32 command: sync
Return all settings in a fixed, parseable format. (<syncItem>:<value>)+

ST25RFAL002/timer.h timerDelay(uint16_t millis)
ST25RFAL002/platform.h platformDelay(millis)
    #define platformDelay(t) furi_delay_ms(t) /*!< Performs a delay for the given time (ms)    */
furi_delay_ms(uint32_t millis)
