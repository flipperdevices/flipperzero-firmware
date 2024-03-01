/**
 * @file start_screen.h
 * GUI: Start Screen view module API
 */

#ifndef START_SCREEN_H
#define START_SCREEN_H

#include <gui/view.h>

#ifdef __cplusplus
extern "C" {
#endif

/** StartScreen anonymous structure */
typedef struct StartScreen StartScreen;

/** StartScreen callback types
 * @warning     comes from GUI thread
 */
typedef void (*StartScreenTimerCallback)(void* context);
typedef bool (*StartScreenInputCallback)(InputEvent* event, void* context);
typedef void (*StartScreenDrawCallback)(Canvas* canvas, void* _model);

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
void start_screen_free(StartScreen* instance);

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

/** Set StartScreen input callback 
 *
 * @param       instance StartScreen instance
 * @param       callback StartScreenInputCallback callback
 */
void start_screen_set_input_callback(StartScreen* instance, StartScreenInputCallback callback);

/** Set Additional draw callback 
 * 
 * Use this as a secondary callback to draw on the canvas.
 * This callback function will happen after drawing of the established
 * GUI elements in the view such as the icons, text, frame, ect.
 *
 * @param       instance StartScreen instance
 * @param callback StartScreenDrawCallback callback
 */
void start_screen_set_secondary_draw_callback(
    StartScreen* instance,
    StartScreenDrawCallback callback);

/** Set StartScreen context 
 *
 * @param       instance StartScreen instance
 * @param       context context pointer, will be passed to callback 
 */
void start_screen_set_context(StartScreen* instance, void* context);

// Maybe have it function similar to widget where you can attach string elements ect
// Right now we will just have a Header, text, and icon similar to popup

/** Set Text1 Element
 *
 * @param      instance StartScreen instance
 * @param      x           x coordinate
 * @param      y           y coordinate
 * @param      horizontal  Align instance
 * @param      vertical    Align instance
 * @param      font        Font instance
 * @param[in]  text        The text
 */
void start_screen_set_text1(
    StartScreen* instance,
    uint8_t x,
    uint8_t y,
    Align horizontal,
    Align vertical,
    Font font,
    const char* text);

/** Set Text2 Element
 *
 * @param      instance StartScreen instance
 * @param      x           x coordinate
 * @param      y           y coordinate
 * @param      horizontal  Align instance
 * @param      vertical    Align instance
 * @param      font        Font instance
 * @param[in]  text        The text
 */
void start_screen_set_text2(
    StartScreen* instance,
    uint8_t x,
    uint8_t y,
    Align horizontal,
    Align vertical,
    Font font,
    const char* text);

/** Set Text3 Element
 *
 * @param      instance StartScreen instance
 * @param      x           x coordinate
 * @param      y           y coordinate
 * @param      horizontal  Align instance
 * @param      vertical    Align instance
 * @param      font        Font instance
 * @param[in]  text        The text
 */
void start_screen_set_text3(
    StartScreen* instance,
    uint8_t x,
    uint8_t y,
    Align horizontal,
    Align vertical,
    Font font,
    const char* text);

/** Set Icon Animation Element
 *
 * @param      instance StartScreen instance
 * @param      x       top left x coordinate
 * @param      y       top left y coordinate
 * @param      animation    IconAnimation instance
 */
void start_screen_set_icon_animation(
    StartScreen* instance,
    uint8_t x,
    uint8_t y,
    const Icon* animation);

#ifdef __cplusplus
}
#endif

#endif
