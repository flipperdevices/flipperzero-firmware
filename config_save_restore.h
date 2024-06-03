/***
 * HC11 modem app
 * Version: 0.1
 *
 * Configuration saving / restoring
***/

/*** Routines ***/

/** Load saved configuration options
    Silently fail **/
void load_configuration(App *);

/** Save configuration options
    Silently fail **/
void save_configuration(App *);
