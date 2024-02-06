#ifndef __UTILS_H__
#define __UTILS_H__

#include "../flipper_structs.h"

/* Select the first scene to show and then block until
 * the GUI stays alive. */
void start_gui_and_block(struct ApplicationContext*);

/* Change scene to the main view after init is finished */
void switch_to_main_scene(struct ApplicationContext*);

/* Go back to main scene that was previously loaded */
void go_back_to_main_scene(struct ApplicationContext*);

/* Send an event to whichever is the active scene at the
 * moment. */
void send_tick_to_scene(struct ApplicationContext*);

/* Show the eat animation and then go back to the previous
 * scene */
void go_to_candy_animation(struct ApplicationContext*);

/* Show the heal animation and then go back to the previous
 * scene */
void go_to_pill_animation(struct ApplicationContext*);

#endif
