#ifndef START_SCREEN
#define START_SCREEN

#include <gui/view.h>
#include <input/input.h>

#ifdef __cplusplus
extern "C" {
#endif

/** StartScreen anonymous structure */
typedef struct StartScreen GameStartScreen;

/** StartScreen timer callback type
 * @warning     comes from GUI thread
 */
typedef (*StartScreenTimerCallback)(void* context);
typedef (*StartScreenInputCallback)();

/** Allocate and initalize
 *
 * This view is used as the starting screen of an application.
 *
 * @return      StartScreen view instance 
 */
StartScreen* start_screen_alloc();

/** Deinitialize and free Start Screen view
 *
 * @param       instsance StartScreen instance
 */
void loading_free(StartScreen* instance);

/** Reset StartScreen
 *
 * @param       instance StartScreen instance
 */
void start_screen_reset(StartScreen* instance);

/** Get StartScreen view
 *
 * @param       instance StartScreen instance
 *
 * @return      view instance that can be used for embedding
 */
View* start_screen_get_view(StartScreen* instance);

/** Set StartScreen timer callback 
 *
 * @param       instance StartScreen instance
 * @param       callback StartScreenTimerCallback callback
 */
void start_screen_set_timer_callback(
        StartScreen* instance,
        StartScreenTimerCallback callback);

/** Set StartScreen input callback 
 *
 * @param       instance StartScreen instance
 * @param       callback StartScreenInputCallback callback
 */
void start_screen_set_input_callback(
        StartScreen* instance,
        StartScreenInputCallback callback);

/** Set StartScreen context 
 *
 * @param       instance StartScreen instance
 * @param       context context pointer, will be passed to callback 
 */
void start_screen_set_context(StartScreen* instance, void* context);

/** Add Multi String Element
 *
 * @param      startScreen StartScreen instance
 * @param      x           x coordinate
 * @param      y           y coordinate
 * @param      horizontal  Align instance
 * @param      vertical    Align instance
 * @param      font        Font instance
 * @param[in]  text        The text
 */
void start_screen_add_string_multiline_element(
    StartScreen* startScreen,
    uint8_t x,
    uint8_t y,
    Align horizontal,
    Align vertical,
    Font font,
    const char* text);

/** Add String Element
 *
 * @param      startScreen StartScreen instance
 * @param      x           x coordinate
 * @param      y           y coordinate
 * @param      horizontal  Align instance
 * @param      vertical    Align instance
 * @param      font        Font instance
 * @param[in]  text        The text
 */
void start_screen_add_string_element(
    StartScreen* startScreen,  
    uint8_t x,
    uint8_t y,
    Align horizontal,
    Align vertical,
    Font font,
    const char* text);

/** Add Icon Element
 *
 * @param      startScreen StartScreen instance
 * @param      x       top left x coordinate
 * @param      y       top left y coordinate
 * @param      icon    Icon instance
 */
void start_screen_add_icon_element(StartScreen* startScreen, uint8_t x, uint8_t y, const Icon* icon);

/** Add Frame Element
 *
 * @param      startScreen StartScreen instance
 * @param      x       top left x coordinate
 * @param      y       top left y coordinate
 * @param      width   frame width
 * @param      height  frame height
 * @param      radius  frame radius
 */
void start_screen_add_frame_element(
    StartScreen* startScreen,
    uint8_t x,
    uint8_t y,
    uint8_t width,
    uint8_t height,
    uint8_t radius);

/** Additional draw callback 
 * 
 * Use this as a secondary callback to draw on the canvas.
 * This callback function will happen after drawing of the established
 * GUI elements in the view such as the icons, text, frame, ect.
 *
 */
void start_screen_set_secondary_draw_callback(StartScreen* startScreen, );
#ifdef __cplusplus
}
#endif
