#pragma once

#include <furi.h>
#include <gui/view_dispatcher.h>

typedef struct GenieApp GenieApp;
typedef struct GenieConfig GenieConfig;
typedef struct GenieSubGhz GenieSubGhz;

ViewDispatcher* genie_app_get_view_dispatcher(GenieApp* app);
GenieConfig* genie_app_get_genie_config(GenieApp* app);
GenieSubGhz* genie_app_get_subghz(GenieApp* app);
void genie_app_start_timer(GenieApp* app, uint32_t ms);
void genie_app_stop_timer(GenieApp* app);
void genie_app_set_frequency(GenieApp* app, uint32_t frequency);
uint32_t genie_app_get_frequency(GenieApp* app);
void genie_app_gpio_send(GenieApp* app, bool sending_signal);
bool genie_app_is_sending_signal(GenieApp* app);
uint32_t genie_app_get_click_counter(GenieApp* app);
void genie_app_increment_click_counter(GenieApp* app);
bool genie_app_has_no_signal(GenieApp* app);
FuriString* genie_app_get_key(GenieApp* app);
const char* genie_app_get_file_path(GenieApp* app);
void genie_app_update_file_path(GenieApp* app, const char* file_path);
void genie_app_update_save_counter(GenieApp* app, uint32_t num_saved);
uint32_t genie_app_get_save_counter(GenieApp* app);
uint32_t genie_app_get_rx_counter(GenieApp* app);
void genie_app_set_processing_packet(GenieApp* app, bool processing_packet);
bool genie_app_is_processing_packet(GenieApp* app);
void genie_app_received_key(GenieApp* app, FuriString* buffer);

void press_button(GenieApp* app);
void release_button(GenieApp* app);

GenieApp* genie_app_alloc();
void genie_app_free(GenieApp* app);