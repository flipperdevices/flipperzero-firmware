/*********************************************
 * Flip.x0 Tutorial
 *               Original by M1ch3al
 *               Modified by HappyAmos to use
 *               an image stored in /images
 * Tip:
 * Check test_app.fam for how to include
 * the images directory in the project, and
 * then include it in your app like:
 *   #include "(app_name)_icons.h" as shown below
 * 0x01 - Test App
 ********************************************
 */

#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>

// Include the icons directory in our project
#include "voltcalc_app_icons.h"

#define TAG "voltcalc_app" // For logging

enum AppStatus {
    STATUS_MAIN,
    STATUS_SELECT_V,
    STATUS_SELECT_R,
    STATUS_SELECT_I,
};

enum ScrollDirection {
	SCROLL_UP,
	SCROLL_NONE,
	SCROLL_DOWN,
};

enum MenuEntryId {
    MNU_I,
    MNU_V,
    MNU_R,
    MNU_ABOUT,
    MNU_CLEAR,
    MNU_RESULT,
};

typedef struct {
  uint8_t id;
  char* text;
} MenuEntry;

#define MENU_ENTRIES 4
const MenuEntry MainMenu[] = {
  {MNU_I, "Current"},
  {MNU_V, "Voltage"},
  {MNU_R, "Resistance"},
  {MNU_ABOUT, "About"},
};

MenuEntry setMenuEntry (uint8_t id, char* text) {
    MenuEntry mnu;
    mnu.id = id;
    mnu.text = text;
    return mnu;
}

/*uint8_t menuSize(const MenuEntry* menu) {
  //uint8_t res = 0;
  //while(menu[res].text != 0) res++;
  //return res;
  uint8_t size = sizeof(menu)/sizeof(menu[0]);
  return size;

}*/

typedef struct {
    float v, r, i;
} VRI;

VRI setVRI (float v, float r, float i) {
    VRI vri;
    vri.v = v;
    vri.r = r;
    vri.i = i;
    return vri;
}

typedef struct {
    FuriMutex* mutex; // Does some magic I have't wrapped my head around yet.
    VRI vri;
    uint8_t status;
    uint8_t currentMenuSelection;
    MenuEntry mainMenu[MENU_ENTRIES];
    uint8_t scroll;
} AppState;


void rotateMenu(AppState* appState) {
    if (appState->scroll == SCROLL_UP) {
      FURI_LOG_I(TAG, "  { scroll up } ");
      MenuEntry tmpEntry = appState->mainMenu[0];
      for (int8_t i = 0; i < MENU_ENTRIES-1; ++i) {
        appState->mainMenu[i] = appState->mainMenu[i+1];
      }
      appState->mainMenu[MENU_ENTRIES - 1] = tmpEntry;
    } else if (appState->scroll == SCROLL_DOWN) {
      FURI_LOG_I(TAG, "  { scroll down } ");
      MenuEntry tmpEntry = appState->mainMenu[MENU_ENTRIES - 1];
      for (int8_t i = MENU_ENTRIES - 1; i > 0; --i) {
        appState->mainMenu[i] = appState->mainMenu[i - 1];
      }
      appState->mainMenu[0] = tmpEntry;
    }

    appState->scroll = SCROLL_NONE;    
}

void draw_triangle(Canvas* canvas, AppState* appState) {

	canvas_set_bitmap_mode(canvas, true);
	canvas_draw_icon(canvas, 5, 15, &I_tri_xxx);

    	appState->mainMenu[1].id == MNU_V ? canvas_set_font(canvas, FontPrimary) : canvas_set_font(canvas, FontSecondary); 
	canvas_draw_str(canvas, 28, 26, "V");
	appState->mainMenu[1].id == MNU_R ? canvas_set_font(canvas, FontPrimary) : canvas_set_font(canvas, FontSecondary); 
	canvas_draw_str(canvas, 20, 38, "R");
	appState->mainMenu[1].id == MNU_I ? canvas_set_font(canvas, FontPrimary) : canvas_set_font(canvas, FontSecondary); 
	canvas_draw_str(canvas, 38, 38, "I");

	/*
	// Triangle
	canvas_set_bitmap_mode(canvas, true);
	canvas_draw_line(canvas, 54, 27, 27, 0);
	canvas_draw_line(canvas, 0, 27, 27, 0);
	canvas_draw_line(canvas, 54, 27, 0, 27);
	canvas_draw_line(canvas, 39, 13, 14, 13);
	canvas_draw_line(canvas, 27, 13, 27, 27);


	if (appState->mainMenu[1].id == MNU_V) {
		// Voltage Highlight
		canvas_draw_line(canvas, 16, 12, 27, 1);
		canvas_draw_line(canvas, 28, 2, 38, 12);
		canvas_draw_line(canvas, 37, 12, 17, 12);
	} else if (appState->mainMenu[1].id == MNU_R) {
		// Resistance Highlight
		canvas_set_bitmap_mode(canvas, true);
		canvas_draw_line(canvas, 14, 14, 1, 27);
		canvas_draw_line(canvas, 14, 14, 26, 14);
		canvas_draw_line(canvas, 26, 27, 26, 15);
		canvas_draw_line(canvas, 25, 27, 2, 27);
	} else if (appState->mainMenu[1].id == MNU_I) {
		// Current Highlight
		canvas_draw_line(canvas, 28, 27, 28, 14);
		canvas_draw_line(canvas, 29, 14, 40, 14);
		canvas_draw_line(canvas, 41, 15, 53, 27);
		canvas_draw_line(canvas, 29, 27, 52, 27);
	}*/

	
}


// Displays a 3 item scrolling menu
void draw_menu(Canvas* canvas, AppState* appState) {

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 65, 13, appState->mainMenu[0].text);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 65, 29, appState->mainMenu[1].text);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 65, 45, appState->mainMenu[2].text);

}

// This function is for drawing the screen GUI, everytime
// the Flip.x0 refresh the display
static void draw_callback(Canvas* canvas, void* ctx) {
    //UNUSED(ctx);
    AppState* appState = (AppState*)ctx;
    furi_mutex_acquire(appState->mutex, FuriWaitForever);

    // This instruction no need details :)
    canvas_clear(canvas);

       // draw button
    canvas_set_font(canvas, FontSecondary);
    elements_button_left(canvas, "Clear");
    canvas_set_font(canvas, FontSecondary);
    elements_button_right(canvas, "Result");

    draw_menu(canvas, appState);
    draw_triangle(canvas, appState);

    furi_mutex_release(appState->mutex);

}

// This function is an handler for the user input (the buttons on the right
// of the Flip.x0 used for navigate, confirm and back)
static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

// Main entry of the application as defined inside the application.fam
int32_t voltcalc_app_main(void* p) {
    UNUSED(p);

    // Current event of type InputEvent
    InputEvent event;

    // Event queue for 8 elements of size InputEvent
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // ViewPort is need to draw the GUI
    ViewPort* view_port = view_port_alloc();

    AppState* appState = malloc(sizeof(AppState));
    appState->status = STATUS_MAIN;
    appState->scroll = SCROLL_NONE;

    FURI_LOG_I(TAG, "Initializing menu, size(%d)", MENU_ENTRIES);
    for (int i = 0; i < MENU_ENTRIES; i++) {
	appState->mainMenu[i] = setMenuEntry(MainMenu[i].id, MainMenu[i].text);
        FURI_LOG_I(TAG, "%d:\tid: %d\t\"%s\"", i, MainMenu[i].id, MainMenu[i].text);
        FURI_LOG_I(TAG, "%d:\tid: %d\t\"%s\"", i, appState->mainMenu[i].id, appState->mainMenu[i].text);
    }

    // we give to this [view_port_draw_callback_set] the function defined
    // before (draw_callback) for drawing the GUI on the Flip.x0 display
    view_port_draw_callback_set(view_port, draw_callback, appState);

    // The same concept is with [view_port_input_callback_set] associated to the (input_callback)
    // function defined before.
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // You need to create a GUI structure and associate it to the viewport previously defined
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Infinite loop...(like arduino or similar)
    while(true) {
        // We continue (indefinitely) to get out of the queue all the events stacked inside
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

	if ((event.type != InputTypeShort) && (event.type != InputTypeLong)) continue;
        furi_mutex_acquire(appState->mutex, FuriWaitForever);

        // If the event from the queue is the press of the back button, we exit from the loop
        if (event.key == InputKeyUp) {
		FURI_LOG_I(TAG, "> Pressed up");
		appState->scroll = SCROLL_DOWN;
		rotateMenu(appState);
	} else if (event.key == InputKeyDown) {
		FURI_LOG_I(TAG, "> Pressed down");
		appState->scroll = SCROLL_UP;
		rotateMenu(appState);
	} else if (event.key == InputKeyOk) {
		FURI_LOG_I(TAG, "> Pressed OK");
		FURI_LOG_I(TAG, "  Selected : %s", appState->mainMenu[1].text);
	} else if (event.key == InputKeyBack) {
		FURI_LOG_I(TAG, "> Pressed back");
            break;
        }

        furi_mutex_release(appState->mutex);

    }

    // once exit from the loop, we need to free resources:
    // clear all the element inside the queue
    furi_message_queue_free(event_queue);

    // We remove the gui from the associated view port
    gui_remove_view_port(gui, view_port);

    // Freeing up memory removing the view_port and close
    // the GUI record
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    return 0;
}
