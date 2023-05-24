// #include "../cfw_app.h"

// enum VarItemListIndex {
// VarItemListIndexShowClock,
// VarItemListIndexShowDate,
// };

// void cfw_app_scene_interface_lockscreen_var_item_list_callback(void* context, uint32_t index) {
// CfwApp* app = context;
// view_dispatcher_send_custom_event(app->view_dispatcher, index);
// }

// static void cfw_app_scene_interface_lockscreen_bad_pins_format_changed(VariableItem* item) {
// CfwApp* app = variable_item_get_context(item);
// bool value = variable_item_get_current_value_index(item);
// variable_item_set_current_value_text(item, value ? "ON" : "OFF");
// CFW_SETTINGS()->bad_pins_format = value;
// app->save_settings = true;
// }

// static void cfw_app_scene_interface_lockscreen_show_time_changed(VariableItem* item) {
// CfwApp* app = variable_item_get_context(item);
// bool value = variable_item_get_current_value_index(item);
// variable_item_set_current_value_text(item, value ? "ON" : "OFF");
// CFW_SETTINGS()->lockscreen_time = value;
// app->save_settings = true;
// }

// static void cfw_app_scene_interface_lockscreen_show_seconds_changed(VariableItem* item) {
// CfwApp* app = variable_item_get_context(item);
// bool value = variable_item_get_current_value_index(item);
// variable_item_set_current_value_text(item, value ? "ON" : "OFF");
// CFW_SETTINGS()->lockscreen_seconds = value;
// app->save_settings = true;
// }

// static void cfw_app_scene_interface_lockscreen_show_date_changed(VariableItem* item) {
// CfwApp* app = variable_item_get_context(item);
// bool value = variable_item_get_current_value_index(item);
// variable_item_set_current_value_text(item, value ? "ON" : "OFF");
// CFW_SETTINGS()->lockscreen_date = value;
// app->save_settings = true;
// }

// static void cfw_app_scene_interface_lockscreen_show_statusbar_changed(VariableItem* item) {
// CfwApp* app = variable_item_get_context(item);
// bool value = variable_item_get_current_value_index(item);
// variable_item_set_current_value_text(item, value ? "ON" : "OFF");
// CFW_SETTINGS()->lockscreen_statusbar = value;
// app->save_settings = true;
// }

// static void cfw_app_scene_interface_lockscreen_unlock_prompt_changed(VariableItem* item) {
// CfwApp* app = variable_item_get_context(item);
// bool value = variable_item_get_current_value_index(item);
// variable_item_set_current_value_text(item, value ? "ON" : "OFF");
// CFW_SETTINGS()->lockscreen_prompt = value;
// app->save_settings = true;
// }

// void cfw_app_scene_interface_lockscreen_on_enter(void* context) {
// CfwApp* app = context;
// CfwSettings* cfw_settings = CFW_SETTINGS();
// VariableItemList* var_item_list = app->var_item_list;
// VariableItem* item;

// item = variable_item_list_add(
// var_item_list,
// "Format on 10 bad PINs",
// 2,
// cfw_app_scene_interface_lockscreen_bad_pins_format_changed,
// app);
// variable_item_set_current_value_index(item, cfw_settings->bad_pins_format);
// variable_item_set_current_value_text(item, cfw_settings->bad_pins_format ? "ON" : "OFF");

// item = variable_item_list_add(
// var_item_list,
// "Show Time",
// 2,
// cfw_app_scene_interface_lockscreen_show_time_changed,
// app);
// variable_item_set_current_value_index(item, cfw_settings->lockscreen_time);
// variable_item_set_current_value_text(item, cfw_settings->lockscreen_time ? "ON" : "OFF");

// item = variable_item_list_add(
// var_item_list,
// "Show Seconds",
// 2,
// cfw_app_scene_interface_lockscreen_show_seconds_changed,
// app);
// variable_item_set_current_value_index(item, cfw_settings->lockscreen_seconds);
// variable_item_set_current_value_text(item, cfw_settings->lockscreen_seconds ? "ON" : "OFF");

// item = variable_item_list_add(
// var_item_list,
// "Show Date",
// 2,
// cfw_app_scene_interface_lockscreen_show_date_changed,
// app);
// variable_item_set_current_value_index(item, cfw_settings->lockscreen_date);
// variable_item_set_current_value_text(item, cfw_settings->lockscreen_date ? "ON" : "OFF");

// item = variable_item_list_add(
// var_item_list,
// "Show Statusbar",
// 2,
// cfw_app_scene_interface_lockscreen_show_statusbar_changed,
// app);
// variable_item_set_current_value_index(item, cfw_settings->lockscreen_statusbar);
// variable_item_set_current_value_text(
// item, cfw_settings->lockscreen_statusbar ? "ON" : "OFF");

// item = variable_item_list_add(
// var_item_list,
// "Unlock Prompt",
// 2,
// cfw_app_scene_interface_lockscreen_unlock_prompt_changed,
// app);
// variable_item_set_current_value_index(item, cfw_settings->lockscreen_prompt);
// variable_item_set_current_value_text(item, cfw_settings->lockscreen_prompt ? "ON" : "OFF");

// variable_item_list_set_enter_callback(
// var_item_list, cfw_app_scene_interface_lockscreen_var_item_list_callback, app);

// variable_item_list_set_selected_item(
// var_item_list,
// scene_manager_get_scene_state(app->scene_manager, CfwAppSceneInterfaceLockscreen));

// view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewVarItemList);
// }

// bool cfw_app_scene_interface_lockscreen_on_event(void* context, SceneManagerEvent event) {
// CfwApp* app = context;
// bool consumed = false;

// if(event.type == SceneManagerEventTypeCustom) {
// scene_manager_set_scene_state(
// app->scene_manager, CfwAppSceneInterfaceLockscreen, event.event);
// consumed = true;
// switch(event.event) {
// default:
// break;
// }
// }

// return consumed;
// }

// void cfw_app_scene_interface_lockscreen_on_exit(void* context) {
// CfwApp* app = context;
// variable_item_list_reset(app->var_item_list);
// }
