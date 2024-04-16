/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.2
 *
 * LRF information view
***/

/*** Includes ***/
#include "noptel_lrf_sampler.h"
#include "noptel_lrf_sampler_icons.h"	/* Generated from images in assets */



/*** Routines ***/

/** LRF identification handler
    Called when a LRF identification frame is available from the LRF serial
    communication app **/
static void lrf_ident_handler(LRFIdent *lrf_ident, void *ctx) {

  App *app = (App *)ctx;
  LRFInfoModel *lrfinfo_model = view_get_model(app->lrfinfo_view);

  /* Copy the identification and mark it as valid */
  memcpy(&(lrfinfo_model->ident), lrf_ident, sizeof(LRFIdent));
  lrfinfo_model->has_ident = true;

  /* Trigger an LRF info view redraw */
  with_view_model(app->lrfinfo_view, LRFInfoModel* _model,
			{UNUSED(_model);}, true);
}



/** LRF info view enter callback **/
void lrfinfo_view_enter_callback(void *ctx) {

  App *app = (App *)ctx;

  /* Setup the callback to receive decoded LRF identification frames */
  set_lrf_ident_handler(app->lrf_serial_comm_app, lrf_ident_handler, app);

  with_view_model(app->lrfinfo_view, LRFInfoModel* lrfinfo_model,
	{
	  /* Invalidate the current identification - if any */
	  lrfinfo_model->has_ident = false;

	  /* Send a send-identification-frame command */
	  send_lrf_command(app->lrf_serial_comm_app, send_ident);
	},
	true);
}



/** LRF info view exit callback **/
void lrfinfo_view_exit_callback(void *ctx) {

  App *app = (App *)ctx;

  /* Unset the callback to receive decoded LRF identification frames */
  set_lrf_ident_handler(app->lrf_serial_comm_app, NULL, app);
}



/** Draw callback for the LRF info view **/
void lrfinfo_view_draw_callback(Canvas *canvas, void *model) {

  LRFInfoModel *lrfinfo_model = (LRFInfoModel *)model;

  /* First print all the things we need to print in the FontPrimary font
     (bold, proportional) */
  canvas_set_font(canvas, FontPrimary);

  /* Do we have a valid identification to display? */
  if(lrfinfo_model->has_ident) {

    /* Draw the identification fields' names */
    canvas_draw_str(canvas, 13, 8, "ID");
    canvas_draw_str(canvas, 3, 17, "Add");
    canvas_draw_str(canvas, 4, 26, "S/N");
    canvas_draw_str(canvas, 2, 35, "F/W");
    canvas_draw_str(canvas, 0, 44, "Date");

    canvas_draw_str(canvas, 90, 26, "Opt");
    canvas_draw_str(canvas, 88, 35, "Elec");
  }

  /* Print the OK button symbol followed by "Read" in a frame at the
     right-hand side */
  canvas_draw_frame(canvas, 77, 52, 51, 12);
  canvas_draw_icon(canvas, 79, 54, &I_ok_button);
  canvas_draw_str(canvas, 102, 62, "Read");

  /* Draw a dividing line between the LRF information and the bottom line */
  canvas_draw_line(canvas, 0, 48, 128, 48);

  /* Do we have a valid identification to display? */
  if(lrfinfo_model->has_ident) {

    /* Second draw the identification values in the FontSecondary font
       (normal, proportional) */
    canvas_set_font(canvas, FontSecondary);

    /* Draw the identification values */
    canvas_draw_str(canvas, 26, 8, lrfinfo_model->ident.id);
    canvas_draw_str(canvas, 26, 17, lrfinfo_model->ident.addinfo);
    canvas_draw_str(canvas, 26, 26, lrfinfo_model->ident.serial);
    canvas_draw_str(canvas, 26, 35, lrfinfo_model->ident.fwversion);
    canvas_draw_str(canvas, 26, 44, lrfinfo_model->ident.builddate);

    canvas_draw_str(canvas, 111, 26, lrfinfo_model->ident.optics);
    canvas_draw_str(canvas, 111, 35, lrfinfo_model->ident.electronics);

  }
}


/** Input callback for the LRF info view **/
bool lrfinfo_view_input_callback(InputEvent *evt, void *ctx) {

  App *app = (App *)ctx;
  LRFInfoModel *lrfinfo_model = view_get_model(app->lrfinfo_view);

  /* If the user pressed the OK button, tell the LRF to send its identification
     information */
  if(evt->type == InputTypePress && evt->key == InputKeyOk) {
    FURI_LOG_I(TAG, "OK button pressed");

    /* Invalidate the current identification - if any */
    lrfinfo_model->has_ident = false;

    /* Trigger an LRF info view redraw to clear the information currently
       displayed - if any */
    with_view_model(app->lrfinfo_view, LRFInfoModel* _model,
			{UNUSED(_model);}, true);

    /* Send a send-identification-frame command */
    send_lrf_command(app->lrf_serial_comm_app, send_ident);

    return true;
  }

  /* We haven't handled this event */
  return false;
}
