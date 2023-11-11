# Flipboard Common Code

This repository contains common code used by Flipboard projects.  Please let us know if you have any questions or comments!

File names are prefixed with the component name, e.g. `app_menu_i.h` is the private header for the `app_menu` component, while `app_menu.h` is the public header.

## app_menu_i.h

This is the private header for the `app_menu` component.  It is included by `app_menu.c` and should not be included by any other files.

This file defines the AppMenu structure.  It is a specializations of the `submenu` component, and is used to display the main menu of the application.

It uses a dynamic ViewIdsArray to store the view ids of the menu items.  The view ids are stored in the order they are added to the menu, and the menu items are displayed in the same order.

## app_menu.c

`app_menu_alloc(ViewDispatcher*)` is used to allocate an AppMenu.  When your application exits, it should call `app_menu_free(AppMenu*)` to free the resources.

`app_menu_add_item(AppMenu*, char* name, View* view, uint32_t view_id)` is used to add a menu item to the menu.  The menu item will be displayed in the order it is added to the menu.

`app_menu_show(AppMenu*)` is used to display the menu.  If the user presses the back button, it will return `VIEW_ID_NONE` (exiting the application).

## app_menu.h

This is the public header for the `app_menu` component.  It is included by `app_menu.c` and should be included by any other files that need to use the `app_menu` component.

`FLIPBOARD_APP_MENU_VIEW_ID` must be 0 and match FlipboardViewAppMenuId.

## backlight.c

`backlight_on` keeps the backlight on even if there is no user interaction.

`backlight_off` turns the backlight off once there is no user interaction.

`backlight_force_off` turns off the backlight.

## backlight.h

This is the public header for the `backlight` component.  It is included by `backlight.c` and should be included by any other files that need to use the `backlight` component.

## config_colors.h

This file defines the colors of the LEDs.  If you add additional colors, please be sure to add to add three locations.  The order of items in color_names and color_values must match.

## config_keystroke.h

This file defines all of the HID keys that can be sent to the host.  Items in `keystroke_names` and `keystroke_values` must match.  There is also a limit to the number of items allowed, so you should not add more than 255 items.

For example, you could remove `F20` and add `LANG 5`; but be sure to update `keystroke_names` and `keystroke_values` to match.

## config_tones.h

This file defines the tones that can be played by the buzzer.  Items in `tone_names` and `tone_values` must match.  There is also a limit to the number of items allowed, so you should not add more than 255 items.

## flipboard_file.c

`flipboard_model_save` saves the model to the file system (SD card/apps_data/flipboard/{appname}.txt).

`flipboard_model_load` loads the model from the file system.

## flipboard_file.h

This is the public header for the `flipboard_file` component.  It is included by `flipboard_file.c` and should be included by any other files that need to use the `flipboard_file` component.

## flipboard_i.h

This is the private header for the `flipboard` component.  It is included by `flipboard.c` and should not be included by any other files.

This file defines the Flipboard structure; which contains the View Dispatcher, the UI components and the FlipboardModel.

This file also defines the FlipboardViewId enum, which is used to identify the views in the application.

## flipboard_model_i.h

This is the private header for the `flipboard_model` component.  It is included by `flipboard_model.c` and should not be included by any other files.

## flipboard_model_ref.h

This is a thin wrapper around FlipboardModel.  It is used to pass a reference to the model to the UI components.