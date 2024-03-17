#include <furi.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>

#define TAG "voltcalc_app"
#define TEXT_INPUT_SIZE 11

typedef enum {
	VoltcalcScenesMainMenuScene,
	VoltcalcScenesVoltageScene,
	VoltcalcScenesResistanceScene,
	VoltcalcScenesCurrentScene,
	VoltcalcScenesCalculateScene,
	VoltcalcScenesAboutScene,
	VoltcalcScenesSceneCount,
} VoltcalcScenesScene;

typedef enum {
	VoltcalcScenesSubmenuView,
	VoltcalcScenesWidgetView,
	VoltcalcScenesTextInputView,
} VoltcalcScenesView;

typedef struct App {
	SceneManager* scene_manager;
	ViewDispatcher* view_dispatcher;
	Submenu* submenu;
	Widget* widget;
	TextInput* text_input;
	char* buffer; // For text input
	uint8_t buffer_size; // for text input
	char V[TEXT_INPUT_SIZE];
	char R[TEXT_INPUT_SIZE];
	char I[TEXT_INPUT_SIZE];
} App;

typedef enum {
	VoltcalcScenesMainMenuSceneVoltage,
	VoltcalcScenesMainMenuSceneResistance,
	VoltcalcScenesMainMenuSceneCurrent,
	VoltcalcScenesMainMenuSceneCalculate,
	VoltcalcScenesMainMenuSceneAbout,
} VoltcalcScenesMainMenuSceneIndex;

typedef enum {
	VoltcalcScenesMainMenuSceneVoltageEvent,
	VoltcalcScenesMainMenuSceneResistanceEvent,
	VoltcalcScenesMainMenuSceneCurrentEvent,
	VoltcalcScenesMainMenuSceneCalculateEvent,
	VoltcalcScenesMainMenuSceneAboutEvent,
} VoltcalcScenesMainMenuEvent;

typedef enum {
	VoltcalcScenesVoltageSceneSaveEvent,
} VoltcalcScenesVoltageEvent;

typedef enum {
	VoltcalcScenesResistanceSceneSaveEvent,
} VoltcalcScenesResistanceEvent;

typedef enum {
	VoltcalcScenesCurrentSceneSaveEvent,
} VoltcalcScenesCurrentEvent;

/* Substitute "_" characters with a "." since
 * the F0 keyboard doesn't include a decimal key. */
void decimal_substitute(char *old) {
	size_t l = strlen(old);
	char *buffer = malloc(l+1);
	strncpy(buffer, old, l);
	for (size_t i = 0; i < l; i++) {
		if (buffer[i] == '_') {
			buffer[i] = '.';
			FURI_LOG_D(TAG, "Found a decimal point substitution character '_'");
			FURI_LOG_D(TAG, "Substitute '.' for '_' %s = %s", old , buffer);
		}
	}
	strncpy(old, buffer, l);
	free(buffer);
}

/* Convert our VRI values to doubles for math purposes, math with them,
 * then convert back to a string. */
void calculate_values(void* context) {
	App* app = context;

	decimal_substitute(app->V);
	decimal_substitute(app->R);
	decimal_substitute(app->I);

	/* Convert the VRI strings to doubles so we can math them */	
	char *ptr1, *ptr2, *ptr3;
	double V, R, I;
	V = strtod(app->V, &ptr1);
	R = strtod(app->R, &ptr2);
	I = strtod(app->I, &ptr3);
	free(ptr1); free(ptr2); free(ptr3); // TODO I don't think this is strictly necessary

	if (V == 0) {
		if (R != 0 && I != 0) {
			V = R * I;
		}
	}
	
	if (R == 0) {
		if (V != 0 && I != 0) {
			R = V / I;
		}
	}

	if (I == 0) {
		if (V != 0 && R != 0) {
			I = V / R;
		}
	}

	// Convert the double values to a string.
	size_t s;
	s = snprintf(NULL, 0, "%lf", V);
	snprintf(app->V, s, "%lf", V);
	s = snprintf(NULL, 0, "%lf", R);
	snprintf(app->R, s, "%lf", R);
	s = snprintf(NULL, 0, "%lf", I);
	snprintf(app->I, s, "%lf", I);

	FURI_LOG_D(TAG, "Voltage double to string: %s", app->V);
	FURI_LOG_D(TAG, "Resistance double to string: %s", app->R);
	FURI_LOG_D(TAG, "Current double to string: %s", app->I);

}


/* Menu stubs */
void voltcalc_scenes_menu_callback(void* context, uint32_t index) {
	App* app = context;
	switch(index) {
		case VoltcalcScenesMainMenuSceneVoltage:
			scene_manager_handle_custom_event(
			app->scene_manager,
			VoltcalcScenesMainMenuSceneVoltageEvent);
			break;
                case VoltcalcScenesMainMenuSceneResistance:
                        scene_manager_handle_custom_event(
                        app->scene_manager,
                        VoltcalcScenesMainMenuSceneResistanceEvent);
                        break;
                case VoltcalcScenesMainMenuSceneCurrent:
                        scene_manager_handle_custom_event(
                        app->scene_manager,
                        VoltcalcScenesMainMenuSceneCurrentEvent);
                        break;
                case VoltcalcScenesMainMenuSceneCalculate:
                        scene_manager_handle_custom_event(
                        app->scene_manager,
                        VoltcalcScenesMainMenuSceneCalculateEvent);
                        break;
                case VoltcalcScenesMainMenuSceneAbout:
                        scene_manager_handle_custom_event(
                        app->scene_manager,
                        VoltcalcScenesMainMenuSceneAboutEvent);
                        break;
	}
}

/* Scene stubs */
/* #main_menu */
void voltcalc_scenes_main_menu_scene_on_enter(void* context) {
	//UNUSED (context);
	App* app = context;
	submenu_reset(app->submenu);
	submenu_set_header(app->submenu, "Voltage Calculator");
	submenu_add_item(app->submenu, "Voltage", VoltcalcScenesMainMenuSceneVoltage, voltcalc_scenes_menu_callback, app);
	submenu_add_item(app->submenu, "Resistance", VoltcalcScenesMainMenuSceneResistance, voltcalc_scenes_menu_callback, app);
	submenu_add_item(app->submenu, "Current", VoltcalcScenesMainMenuSceneCurrent, voltcalc_scenes_menu_callback, app);
	submenu_add_item(app->submenu, "Calculate", VoltcalcScenesMainMenuSceneCalculate, voltcalc_scenes_menu_callback, app);
	submenu_add_item(app->submenu, "About", VoltcalcScenesMainMenuSceneAbout, voltcalc_scenes_menu_callback, app);
	view_dispatcher_switch_to_view(app->view_dispatcher, VoltcalcScenesSubmenuView);
}

bool voltcalc_scenes_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case VoltcalcScenesMainMenuSceneVoltageEvent:
            scene_manager_next_scene(app->scene_manager, VoltcalcScenesVoltageScene);
            consumed = true;
            break;
        case VoltcalcScenesMainMenuSceneResistanceEvent:
            scene_manager_next_scene(app->scene_manager, VoltcalcScenesResistanceScene);
            consumed = true;
            break;
        case VoltcalcScenesMainMenuSceneCurrentEvent:
            scene_manager_next_scene(app->scene_manager, VoltcalcScenesCurrentScene);
            consumed = true;
            break;
        case VoltcalcScenesMainMenuSceneCalculateEvent:
            scene_manager_next_scene(app->scene_manager, VoltcalcScenesCalculateScene);
            consumed = true;
            break;
        case VoltcalcScenesMainMenuSceneAboutEvent:
            scene_manager_next_scene(app->scene_manager, VoltcalcScenesAboutScene);
            consumed = true;
            break;
        }
        break;
    default:
        break;
    }
    return consumed;
}

void voltcalc_scenes_main_menu_scene_on_exit(void* context) {
	//UNUSED (context);
	App* app = context;
	submenu_reset(app->submenu);
}

void voltcalc_scenes_voltage_callback(void* context) {
	App* app = context;
	scene_manager_handle_custom_event(app->scene_manager, 
		VoltcalcScenesVoltageSceneSaveEvent);
}

void voltcalc_scenes_resistance_callback(void* context) {
	App* app = context;
	scene_manager_handle_custom_event(app->scene_manager,
		VoltcalcScenesResistanceSceneSaveEvent);
}

void voltcalc_scenes_current_callback(void* context) {
	App* app = context;
	scene_manager_handle_custom_event(app->scene_manager,
		VoltcalcScenesCurrentSceneSaveEvent);
}

/* #voltage */
void voltcalc_scenes_voltage_scene_on_enter(void* context) {
	App* app = context;
	bool clear_text = true;
	strcpy(app->buffer, app->V); // Copy the result from the keyboard into our context
	text_input_reset(app->text_input);
	text_input_set_header_text(app->text_input, "Enter voltage:");
	text_input_set_result_callback(
		app->text_input,
		voltcalc_scenes_voltage_callback,
		app,
		app->buffer,
		app->buffer_size,
		clear_text);
	view_dispatcher_switch_to_view(app->view_dispatcher, VoltcalcScenesTextInputView);
}

bool voltcalc_scenes_voltage_scene_on_event(void* context, SceneManagerEvent event) {
	App* app = context;
	bool consumed = false;
	if(event.type == SceneManagerEventTypeCustom) {
		if(event.event == VoltcalcScenesVoltageSceneSaveEvent) {
			//scene_manager_next_scene(app->scene_manager, VoltcalcScenesCalculateScene);
			FURI_LOG_D(TAG, "Clicked save");
			strcpy(app->V, app->buffer);
			calculate_values(app);
			consumed = true;
			scene_manager_previous_scene(app->scene_manager);
		}
	}
	return consumed;
}

void voltcalc_scenes_voltage_scene_on_exit(void* context) {
        UNUSED (context);
}

/* #resistance */
void voltcalc_scenes_resistance_scene_on_enter(void* context) {
	App* app = context;
	bool clear_text = true;
	strcpy(app->buffer, app->R); // Copy the value from the keyboard into our context
	text_input_reset(app->text_input);
	text_input_set_header_text(app->text_input, "Enter resistance:");
	text_input_set_result_callback(
		app->text_input,
		voltcalc_scenes_resistance_callback,
		app,
		app->buffer,
		app->buffer_size,
		clear_text);
	view_dispatcher_switch_to_view(app->view_dispatcher, VoltcalcScenesTextInputView);
}

bool voltcalc_scenes_resistance_scene_on_event(void* context, SceneManagerEvent event) {
	App* app = context;
	bool consumed = false;
	if(event.type == SceneManagerEventTypeCustom) {
		if(event.event == VoltcalcScenesResistanceSceneSaveEvent) {
			//scene_manager_next_scene(app->scene_manager, VoltcalcScenesCalculateScene);
			FURI_LOG_D(TAG, "Clicked save");
			strcpy(app->R, app->buffer);
			calculate_values(app);
			consumed = true;
			scene_manager_previous_scene(app->scene_manager);
		}
	}
	return consumed;

}

void voltcalc_scenes_resistance_scene_on_exit(void* context) {
        UNUSED (context);
}

/* #current */
void voltcalc_scenes_current_scene_on_enter(void* context) {
	App* app = context;
	bool clear_text = true;
	strcpy(app->buffer, app->I); // Copy the value from the keyboard into our context.
	text_input_reset(app->text_input);
	text_input_set_header_text(app->text_input, "Enter current:");
	text_input_set_result_callback(
		app->text_input,
		voltcalc_scenes_current_callback,
		app,
		app->buffer,
		app->buffer_size,
		clear_text);
	view_dispatcher_switch_to_view(app->view_dispatcher, VoltcalcScenesTextInputView);
}

bool voltcalc_scenes_current_scene_on_event(void* context, SceneManagerEvent event) {
	App* app = context;
	bool consumed = false;
	if(event.type == SceneManagerEventTypeCustom) {
		if(event.event == VoltcalcScenesCurrentSceneSaveEvent) {
			//scene_manager_next_scene(app->scene_manager, VoltcalcScenesCalculateScene);
			FURI_LOG_D(TAG, "Clicked save");
			strcpy(app->I, app->buffer);
			calculate_values(app);
			consumed = true;
			scene_manager_previous_scene(app->scene_manager);
		}
	}
	return consumed;
}

void voltcalc_scenes_current_scene_on_exit(void* context) {
        UNUSED (context);
}

/* #calculate */
void voltcalc_scenes_calculate_scene_on_enter(void* context) {
        //UNUSED (context);
        App* app = context;
	widget_reset(app->widget);
	FuriString* message = furi_string_alloc(); // for concat?

	furi_string_printf(message, "Voltage: %s", app->V);
	widget_add_string_element(
		app->widget, 
		5, 5, 
		AlignLeft, AlignCenter, 
		FontPrimary,
		furi_string_get_cstr(message));
	furi_string_printf(message, "Resistance: %s", app->R);
	widget_add_string_element(
		app->widget, 
		5, 15, 
		AlignLeft, AlignCenter, 
		FontPrimary,
		furi_string_get_cstr(message));
	furi_string_printf(message, "Current: %s", app->I);
	widget_add_string_element(
		app->widget, 
		5, 25, 
		AlignLeft, AlignCenter, 
		FontPrimary,
		furi_string_get_cstr(message));
	furi_string_free(message);
	view_dispatcher_switch_to_view(app->view_dispatcher, VoltcalcScenesWidgetView);

}

bool voltcalc_scenes_calculate_scene_on_event(void* context, SceneManagerEvent event) {
        UNUSED (context);
        UNUSED (event);
        return false; // event not handled
}

void voltcalc_scenes_calculate_scene_on_exit(void* context) {
        UNUSED (context);
}

/*
 * https://github.com/flipperdevices/flipperzero-firmware/blob/dev/applications/services/gui/modules/widget.h
 */
/* #about */
void voltcalc_scenes_about_scene_on_enter(void* context) {
        //UNUSED (context);
        App* app = context;
	widget_reset(app->widget);
	widget_add_text_scroll_element(
		app->widget, 1, 1, 128, 64,
		"\e#About\n\nVRI Calculator\n\nEnter at least two values\nand the app will solve the\nthird. Use '_' for a\ndecimal.\n\n(c) Andrew Diamond\n\nSpecial thanks to Derek\nJamison for his flipper\ntutorials.\n https://github.com/jamisonderek\n\nA large thank you to the\nrest of the F0 developer\ncommunity that have taken\nthe time to help others\nwith their knowledge.\n");
	view_dispatcher_switch_to_view(app->view_dispatcher, VoltcalcScenesWidgetView);
}

bool voltcalc_scenes_about_scene_on_event(void* context, SceneManagerEvent event) {
        UNUSED (context);
        UNUSED (event);
        return false; // event not handled
}

void voltcalc_scenes_about_scene_on_exit(void* context) {
        UNUSED (context);
}

/* STUB sample
void voltcalc_scenes_XXX_scene_on_enter(void* context) {
        UNUSED (context);
}

bool voltcalc_scenes_XXX_scene_on_event(void* context, SceneManagerEvent event) {
        UNUSED (context);
        UNUSED (event);
        return false; // event not handled
}

void voltcalc_scenes_XXX_scene_on_exit(void* context) {
        UNUSED (context);
}
*/


/* on_enter handlers */
void (*const voltcalc_scenes_scene_on_enter_handlers[])(void*) = {
    voltcalc_scenes_main_menu_scene_on_enter,
    voltcalc_scenes_voltage_scene_on_enter,
    voltcalc_scenes_resistance_scene_on_enter,
    voltcalc_scenes_current_scene_on_enter,
    voltcalc_scenes_calculate_scene_on_enter,
    voltcalc_scenes_about_scene_on_enter,
};

/* on_event handlers */
bool (*const voltcalc_scenes_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    voltcalc_scenes_main_menu_scene_on_event,
    voltcalc_scenes_voltage_scene_on_event,
    voltcalc_scenes_resistance_scene_on_event,
    voltcalc_scenes_current_scene_on_event,
    voltcalc_scenes_calculate_scene_on_event,
    voltcalc_scenes_about_scene_on_event,
};

/* on_exit handlers */
void (*const voltcalc_scenes_scene_on_exit_handlers[])(void*) = {
    voltcalc_scenes_main_menu_scene_on_exit,
    voltcalc_scenes_voltage_scene_on_exit,
    voltcalc_scenes_resistance_scene_on_exit,
    voltcalc_scenes_current_scene_on_exit,
    voltcalc_scenes_calculate_scene_on_exit,
    voltcalc_scenes_about_scene_on_exit,
};

/* Scene Manager callback */
static const SceneManagerHandlers voltcalc_scenes_scene_manager_handlers = {
    .on_enter_handlers = voltcalc_scenes_scene_on_enter_handlers,
    .on_event_handlers = voltcalc_scenes_scene_on_event_handlers,
    .on_exit_handlers = voltcalc_scenes_scene_on_exit_handlers,
    .scene_num = VoltcalcScenesSceneCount,
};

/* app scene_custom_callback */
static bool voltcalc_scene_custom_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

/* scene_back_event */
bool voltcalc_scene_back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static App* app_alloc() {
    App* app = malloc(sizeof(App));
    app->buffer_size = TEXT_INPUT_SIZE;
    app->buffer = malloc(app->buffer_size);
    app->scene_manager = scene_manager_alloc(
      &voltcalc_scenes_scene_manager_handlers,
      app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, voltcalc_scene_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, voltcalc_scene_back_event_callback);
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        VoltcalcScenesSubmenuView,
        submenu_get_view(app->submenu)); 
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        VoltcalcScenesWidgetView,
        widget_get_view(app->widget));
    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        VoltcalcScenesTextInputView,
        text_input_get_view(app->text_input));
    return app;
}

/* free resources */
static void app_free(App* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->view_dispatcher, VoltcalcScenesSubmenuView);
    view_dispatcher_remove_view(app->view_dispatcher, VoltcalcScenesWidgetView);
    view_dispatcher_remove_view(app->view_dispatcher, VoltcalcScenesTextInputView);
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    widget_free(app->widget);
    text_input_free(app->text_input);
    free(app->buffer);
    free(app);
}

/* Give our VRI variables a starting value of zero. */
static void app_initialize(App* app) {
    strcpy(app->V, "0");
    strcpy(app->R, "0");
    strcpy(app->I, "0");
    calculate_values(app);
}

int32_t voltcalc_app(void* p) {
    UNUSED(p);
    App* app = app_alloc();
    app_initialize(app);

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
      app->view_dispatcher,
      gui,
      ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(
      app->scene_manager,
      VoltcalcScenesMainMenuScene);
    view_dispatcher_run(app->view_dispatcher);

    app_free(app);
    return 0;
}
