/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.5
 *
 * Submenu
***/

/*** Includes ***/
#include "noptel_lrf_sampler.h"
#include "submenu.h"



/*** Routines ***/

/** Handle submenu views switching **/
void submenu_callback(void *ctx, uint32_t idx) {

  App *app = (App *)ctx;
  SamplerModel *sampler_model = view_get_model(app->sample_view);

  switch(idx) {

    /* Switch to the configuration view */
    case submenu_config:
      view_dispatcher_switch_to_view(app->view_dispatcher, view_config);
      sampler_model->config.sitem = submenu_config;
      FURI_LOG_D(TAG, "Switch to configuration view");
      break;

    /* Switch to the sample view */
    case submenu_sample:
      view_dispatcher_switch_to_view(app->view_dispatcher, view_sample);
      sampler_model->config.sitem = submenu_sample;
      FURI_LOG_D(TAG, "Switch to sample view");
      break;

    /* Turn the pointer on and off */
    case submenu_pointeronoff:
      send_lrf_command(app->lrf_serial_comm_app,
			sampler_model->pointer_is_on? pointer_off : pointer_on,
			NULL, 0);
      FURI_LOG_D(TAG, "Turned the pointer %s",
			sampler_model->pointer_is_on? "OFF" : "ON");
      sampler_model->config.sitem = submenu_pointeronoff;
      sampler_model->pointer_is_on = !sampler_model->pointer_is_on;
      break;

    /* Switch to the LRF info view */
    case submenu_lrfinfo:
      view_dispatcher_switch_to_view(app->view_dispatcher, view_lrfinfo);
      sampler_model->config.sitem = submenu_lrfinfo;
      FURI_LOG_D(TAG, "Switch to LRF info view");
      break;

    /* Switch to the save diagnostic view */
    case submenu_savediag:
      view_dispatcher_switch_to_view(app->view_dispatcher, view_savediag);
      sampler_model->config.sitem = submenu_savediag;
      FURI_LOG_D(TAG, "Switch to save diagnostic view");
      break;

    /* Switch to the test laser view */
    case submenu_testlaser:
      view_dispatcher_switch_to_view(app->view_dispatcher, view_testlaser);
      sampler_model->config.sitem = submenu_testlaser;
      FURI_LOG_D(TAG, "Switch to test laser view");
      break;

    /* Switch to the test pointer view */
    case submenu_testpointer:
      view_dispatcher_switch_to_view(app->view_dispatcher, view_testpointer);
      sampler_model->config.sitem = submenu_testpointer;
      FURI_LOG_D(TAG, "Switch to test pointer view");
      break;

    /* Switch to the about view */
    case submenu_about:
      view_dispatcher_switch_to_view(app->view_dispatcher, view_about);
      sampler_model->config.sitem = submenu_about;
      FURI_LOG_D(TAG, "Switch to about view");
      break;

    default:
      break;
    }
}



/** Callback to return to the submenu **/
uint32_t return_to_submenu_callback(void *ctx) {

  UNUSED(ctx);

  /* Switch back to the submenu */
  FURI_LOG_D(TAG, "Switch back to submenu");
  return view_submenu;
}



/* Callback to exit the submenu and the app altogether */
uint32_t submenu_exit_callback(void *ctx) {

  UNUSED(ctx);

  /* Set the view to VIEW_NONE to exit */
  FURI_LOG_D(TAG, "Exit");
  return VIEW_NONE;
}
