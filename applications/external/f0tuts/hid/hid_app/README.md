# Cookie Clicker Tutorial

I recommend the [hid_cookie](../hid_cookie/readme.md) tutorial instead.

```
```

This tutorial modifies the Flipper Zero firmware to add a Cookie Clicker controller option to the hid_app (known as "Remote" under Applications/Bluetooth). This Cookie clicker will automatically click the mouse and allow you to set the speed of the clicks.

The "Remote" application is a Bluetooth HID (Human Interface Device) application. It allows you to connect to a Bluetooth HID device and send keystrokes and mouse clicks. The Flipper Zero firmware has a TikTok controller built in. This tutorial will show you how to modify the firmware to add a Cookie Clicker controller, starting with the code from the TikTok controller.

Make sure you have a working Flipper Zero before you start this tutorial & that
you have enabled Bluetooth on the Flipper Zero (under Settings). You will need to pair the Flipper Zero with your phone before you can use the Cookie Clicker controller (the Flipper Zero device should display in your phone's Bluetooth settings while the "Remote" application is running).

If you are unclear of a step, the [final_files](./final_files/) folder contains the modified files you should have after you have completed the tutorial. Please let me know which step was unclear and I will try to clarify it for future users.

## Step 0. Another tutorial can be found at hid_cookie
I recommend the [hid_cookie](../hid_cookie/readme.md) tutorial instead.



## Step 1. Install Git and VS Code.

If you have not already installed Git and VS Code, you will need to do so. The following links will take you to the download pages for Git and VS Code.

- [Git](https://git-scm.com/downloads)
- [VS Code](https://code.visualstudio.com/download)

## Step 2. Clone the Flipper Zero firmware.

Clone the Flipper Zero firmware from GitHub. The following command will clone the firmware into a folder named official-firmware. (The below commands may wrap on your screen. You can copy and paste the entire command; there should only be two lines.)

Open a command window (in windows, press the Windows key+R, type: cmd, then press enter)
```console
md <your working directory>
cd <your working directory>
git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git official-firmware
```

Replace _&lt;your working directory&gt;_ with the directory where you want to clone the firmware (like \flipper).

## Step 3. Run FBT to build the firmware and configure VS Code.

Run the following commands from the root of the firmware folder to build the firmware and configure VS Code. Replace _&lt;your working directory&gt;_ with the directory where you cloned the firmware. In some environments you do not need the "./" at the beginning of the command.

```console
cd <your working directory>
cd official-firmware
./fbt vscode_dist
./fbt updater_package
```

\*\*\* **Please follow the steps at [FBT VSCode integration](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/fbt.md#vscode-integration) before proceeding.** \*\*\*

## Step 4. Open the applications\external\hid_app folder in VS Code.

After following the previous step, navigate to the **applications \ external \ hid_app** folder in VS Code.

## Step 5. Expand the assets folder.

Expand the **assets** folder in VS Code.

## Step 6. Create a Cookie_def_11x9.png.

Copy the **Like_def_11x9.png** file to **Cookie_def_11x9.png**.
Edit the file to have a cookie image.

![11x9 cookie](https://www.github.com/jamisonderek/flipper-zero-tutorials/blob/main/hid/hid_app/final_files/assets/Cookie_def_11x9.png?raw=true)

## Step 7. Create a Cookie_pressed_17x17.png.

Copy the **Like_pressed_17x17.png** file to **Cookie_pressed_17x17.png**.
Edit the file to have a cookie image.

![17x17 cookie](https://www.github.com/jamisonderek/flipper-zero-tutorials/blob/main/hid/hid_app/final_files/assets/Cookie_pressed_17x17.png?raw=true)

## Step 8. Expand the views folder.

Expand the **views** folder in VS Code.

## Step 9. Create a hid_cc.h file.

In the views folder, copy the **hid_tiktok.h** file to **hid_cc.h**.

## Step 10. Edit the hid_cc.h file.

Edit the **hid_cc.h** file to have the following changes:

- Case-sensitive! Replace all TikTok with CC
- Case-sensitive! Replace all \_tiktok with \_cc

## Step 11. Create a hid_cc.c file.

In the views folder, copy the **hid_tiktok.c** file to **hid_cc.c**.

## Step 12. Edit the hid_cc.c file.

Edit the **hid_cc.c** file to have the following changes:

- Case-sensitive! Replace all TikTok with CC
- Case-sensitive! Replace all \_tiktok with \_cc
- Case-sensitive! Replace all I_Like\_ with I_Cookie\_

## Step 13. Edit the views.h file.

Edit the **views.h** file to have the following changes:

- Below **BtHidViewTikTok,** add the following line:

```c
    BtHidViewCC,
```

## Step 14. Edit the hid.h file.

Edit the **hid.h** file to have the following changes:

- Below **#include "views/hid_tiktok.h"** add the following line:

```c
#include "views/hid_cc.h"
```

- Below **HidTikTok\* hid_tiktok;** add the following line:

```c
HidCC* hid_cc;
```

## Step 15. Edit the hid.c file.

Edit the **hid.c** file to have the following changes:

- Below **HidSubmenuIndexTikTok,** add the following line:

```c
    HidSubmenuIndexCC,
```

- Below **view_dispatcher_switch_to_view(app->view_dispatcher, BtHidViewTikTok);** add the following lines:

```c
    } else if(index == HidSubmenuIndexCC) {
        app->view_id = BtHidViewCC;
        view_dispatcher_switch_to_view(app->view_dispatcher, BtHidViewCC);
```

- Below **hid_tiktok_set_connected_status(hid->hid_tiktok, connected);** add the following line:

```c
hid_cc_set_connected_status(hid->hid_cc, connected);
```

- Find the text "TikTok Controller" and then after the next close bracket (a few lines later), add the following commands:

```c
    if(app->transport == HidTransportBle) {
        submenu_add_item(
            app->device_type_submenu,
            "CC Controller",
            HidSubmenuIndexCC,
            hid_submenu_callback,
            app);
    }
```

- Find the text "TikTok view" and then after the next blank line (a few lines later), add the following commands:

```c
    // CC view
    app->hid_cc = hid_cc_alloc(app);
    view_set_previous_callback(hid_cc_get_view(app->hid_cc), hid_exit_confirm_view);
    view_dispatcher_add_view(app->view_dispatcher, BtHidViewCC, hid_cc_get_view(app->hid_cc));
```

- After the line **hid_tiktok_free(hid->hid_tiktok);** add the following commands:

```c
    view_dispatcher_remove_view(app->view_dispatcher, BtHidViewCC);
    hid_cc_free(app->hid_cc);
```

## Step 16. Compile and run the application

You should now how copied the TikTok application to the Cookie Clicker application. Lets build and deploy the firmware to your Flipper Zero,
and confirm that "Cookie Clicker" is just a copy of TikTok.

Run the following command from the root of the firmware folder to compile and run the application. Make sure you are not currently running the Flipper UI,
qFlipper or lab.flipper.net. In some environments you do not need the "./" at the beginning of the command.

```console
cd <your working directory>
cd official-firmware
./fbt launch_app APPSRC=applications/external/hid_app
```

The application should run on the Flipper Zero and you should see the Cookie Clicker application. You can now edit the application to add your own
custom code.

## Step 17. Decide what changes we need to make to our view (hid_cc.c)

We need to make the following changes to our hid_cc view:

- Add a timer that fires every so often.
- Keep track of the click speed and if the timer is enabled.
- Display the speed of the cookie clicker.
- When the timer fires, we need to click the mouse if enabled.
- When UP is pressed, increase the speed of the cookie clicker.
- When DOWN is pressed, decrease the speed of the cookie clicker.
- When OK is pressed, toggle the timer on/off.

## Step 18. Add a "FuriTimer\* timer" to the HidCC:

In the **hid_cc.c** file, find modify the **struct HidCC** to have the following changes:

```c
struct HidCC {
View* view;
Hid* hid;
FuriTimer* timer; // Add this line
};
```

## Step 19. Add a "float timer_duration" and "bool timer_click_enabled" to HidCCModel.

In the **hid_cc.c** file, find modify the **HidCCModel** to have the following changes:

```c
typedef struct {
bool left_pressed;
bool up_pressed;
bool right_pressed;
bool down_pressed;
bool ok_pressed;
bool connected;
bool is_cursor_set;
HidTransport transport;
float timer_duration; // Add this line (duration in ms)
bool timer_click_enabled; // Add this line (are we clicking)
} HidCCModel;
```

## Step 20. Rename title from "CC" to "Cookie\nClicker".

Replace the title which should be double-quotes followed by CC followed by double-quotes. We use \n so the text will wrap 2 lines. Find the line **elements_multiline_text_aligned(canvas, 17, 3, AlignLeft, AlignTop, "CC");** which contains the title and change it to:

```c
elements_multiline_text_aligned(
canvas, 17, 3, AlignLeft, AlignTop, "Cookie\nClicker");
```

## Step 21. Add code to display the click speed, right after displaying the title.

The **canvas_set_font(canvas, FontSecondary);** should already be the text immediately after the title. We will add the code to display the click speed
right after this line. Find the line **canvas_set_font(canvas, FontSecondary);** and add the following code:

```c
canvas_set_font(canvas, FontSecondary);
FuriString\* buffer = furi_string_alloc(32);
furi_string_printf(
buffer,
"%0.1f ms\r\n",
(double)model->timer_duration);
elements_multiline_text_aligned(canvas, 17, 25, AlignLeft, AlignTop, furi_string_get_cstr(buffer));
furi_string_free(buffer);
```

This code does the following:

- Sets the font to FontSecondary.
- Allocates a buffer to hold the text.
- Formats the text (containing the duration) into the buffer.
- Displays the text on the screen.
- Frees the buffer.

## Step 22. Add a new method to update the timer.

-You can add this code immediate before the **static void hid_cc_reset_cursor(HidCC\* hid_cc)** code. Find the line **static void hid_cc_reset_cursor(HidCC\* hid_cc)** and add the following _above_ it:

```c
static void hid_cc_update_timer(HidCC* hid_cc, HidCCModel* model) {
    furi_assert(hid_cc);
    furi_assert(model);
    if(model->timer_click_enabled) {
        furi_timer_start(hid_cc->timer, model->timer_duration);
    } else {
        furi_timer_stop(hid_cc->timer);
    }
}
```

This code does the following:

- Checks that the hid_cc and model are not null.
- If the timer is enabled, start the timer using the duration from the model.
- If the timer is disabled, stop the timer.

## Step 23. Add a new method to handle when the timer fires.

- You can add this code immediately after the code from the previous step.

```c
static void hid_cc_tick_callback(void* context) {
    furi_assert(context);
    HidCC* hid_cc = context;

    with_view_model(
        hid_cc->view,
        HidCCModel * model,
        {
            if (model->timer_click_enabled) {
            hid_hal_mouse_press(hid_cc->hid, HID_MOUSE_BTN_LEFT);
            furi_delay_ms(10);
            hid_hal_mouse_release(hid_cc->hid, HID_MOUSE_BTN_LEFT);
            }
        },
        true);
}
```

This code does the following:

- Checks that the context is not null.
- Gets the model from the view.
- If the timer is enabled, click the mouse.

## Step 24, change what the OK button does.

We will update the hid_cc_input_callback handling of short press of the OK button. We want the code to toggle the timer_click_enabled value and then update_timer (so it starts/stops).

REPLACE:

```c
if(event->key == InputKeyOk) {
hid_hal_mouse_press(hid_tiktok->hid, HID_MOUSE_BTN_LEFT);
furi_delay_ms(50);
hid_hal_mouse_release(hid_tiktok->hid, HID_MOUSE_BTN_LEFT);
furi_delay_ms(50);
hid_hal_mouse_press(hid_tiktok->hid, HID_MOUSE_BTN_LEFT);
furi_delay_ms(50);
hid_hal_mouse_release(hid_tiktok->hid, HID_MOUSE_BTN_LEFT);
consumed = true;
}
```

WITH:

```c
if(event->key == InputKeyOk) {
  model->timer_click_enabled = !model->timer_click_enabled;
  hid_cc_update_timer(hid_cc, model);
  consumed = true;
}
```

## Step 25, change what the UP button does.

We will update the hid_cc_input_callback handling of short press of the UP button. We want the code to reduce the duration of the timer to 95% of value + update_timer.

REPLACE:
```c
else if(event->key == InputKeyUp) {
// Emulate up swipe
hid_hal_mouse_scroll(hid_tiktok->hid, -6);
hid_hal_mouse_scroll(hid_tiktok->hid, -12);
hid_hal_mouse_scroll(hid_tiktok->hid, -19);
hid_hal_mouse_scroll(hid_tiktok->hid, -12);
hid_hal_mouse_scroll(hid_tiktok->hid, -6);
consumed = true;
````

WITH:

```c
} else if(event->key == InputKeyUp) {
  // Reduce duration to 95% of value + update_timer.
  if (model->timer_duration>0) {
    model->timer_duration *= 0.95f;
    hid_cc_update_timer(hid_cc, model);
  }
  consumed = true;
```

## Step 26, change what the DOWN button does.

We will update the hid_cc_input_callback handling of short press of the DOWN button. We want the code to increase the duration of the timer to 105% of value + update_timer.

REPLACE:

```c
} else if(event->key == InputKeyDown) {
// Emulate down swipe
hid_hal_mouse_scroll(hid_tiktok->hid, 6);
hid_hal_mouse_scroll(hid_tiktok->hid, 12);
hid_hal_mouse_scroll(hid_tiktok->hid, 19);
hid_hal_mouse_scroll(hid_tiktok->hid, 12);
hid_hal_mouse_scroll(hid_tiktok->hid, 6);
consumed = true;
```

WITH:

```c
} else if(event->key == InputKeyDown) {
  // Increase duration to 105% of value + update_timer
  model->timer_duration *= 1.05f;
  hid_cc_update_timer(hid_cc, model);
  consumed = true;
```

## Step 27, allocate the timer object.

Update hid_cc_alloc to set timer values & allocate timer object.

REPLACE:

```c
with_view_model(
hid_cc->view, HidCCModel * model, { model->transport = bt*hid->transport; }, true);
```

WITH:

```c
with_view_model(
hid_cc->view,
HidCCModel * model,
{
model->transport = bt_hid->transport;
// Set default values for timer
model->timer_duration = 22.0f;
model->timer_click_enabled = false;
},
true);
// Allocate timer
hid_cc->timer = furi_timer_alloc(hid_cc_tick_callback, FuriTimerTypePeriodic, hid_cc);
```

This code does the following:

- Gets the model from the view.
- Sets the default values for the timer (22ms and not enabled).
- Allocates the timer object.

## Step 28, free the timer object.

Update hic_cc_free to free timer object.

REPLACE:

```c
view_free(hid_cc->view);
```

WITH:

```c
furi_timer_free(hid_cc->timer);
view_free(hid_cc->view);
```

## Step 29. Compile and run the application

You should now how copied the TikTok application to the Cookie Clicker application. Lets build and deploy the firmware to your Flipper Zero,
and confirm that "Cookie Clicker" is just a copy of TikTok.

Run the following command from the root of the firmware folder to compile and run the application. Make sure you are not currently running the Flipper UI,
qFlipper or lab.flipper.net. In some environments you do not need the "./" at the beginning of the command.

```console
cd <your working directory>
cd official-firmware
./fbt launch_app APPSRC=applications/external/hid_app
```

The application should run on the Flipper Zero and you should see the Cookie Clicker application. When you click the OK button, the timer should start and stop. When you click the UP button, the timer duration should decrease. When you click the DOWN button, the timer duration should increase.

Congratulations! You have successfully modified the TikTok application to the Cookie Clicker application.