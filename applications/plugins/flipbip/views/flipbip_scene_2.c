// #include "../flipbip.h"
// #include <furi.h>
// #include <furi_hal.h>
// #include <input/input.h>
// #include <gui/elements.h>
// #include <dolphin/dolphin.h>
// #include "../helpers/flipbip_haptic.h"
// //#include "../helpers/flipbip_speaker.h"
// #include "../helpers/flipbip_led.h"

// struct FlipBipScene2 {
//     View* view;
//     FlipBipScene2Callback callback;
//     void* context;
// };

// typedef struct {
//     int screen_text;
// } FlipBipScene2Model;

// char buttonText[11][14] = {
//     "",
//     "Press Up",
//     "Press Down",
//     "Press Left",
//     "Press Right",
//     "Press Ok",
//     "Release Up",
//     "Release Down",
//     "Release Left",
//     "Release Right",
//     "Release Ok",
// };

// void flipbip_scene_2_set_callback(
//     FlipBipScene2* instance,
//     FlipBipScene2Callback callback,
//     void* context) {
//     furi_assert(instance);
//     furi_assert(callback);
//     instance->callback = callback;
//     instance->context = context;
// }

// void flipbip_scene_2_draw(Canvas* canvas, FlipBipScene2Model* model) {
//     canvas_clear(canvas);
//     canvas_set_color(canvas, ColorBlack);
//     canvas_set_font(canvas, FontPrimary);
//     canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, "Scene 2: Input Examples");
//     canvas_set_font(canvas, FontSecondary);
//     char *strInput = malloc(15);
//     strcpy(strInput, buttonText[model->screen_text]);
//     canvas_draw_str_aligned(canvas, 0, 22, AlignLeft, AlignTop, strInput);
//     free(strInput);
// }

// static void flipbip_scene_2_model_init(FlipBipScene2Model* const model) {
//     model->screen_text = 0;
// }

// bool flipbip_scene_2_input(InputEvent* event, void* context) {
//     furi_assert(context);
//     FlipBipScene2* instance = context;
//     if (event->type == InputTypeRelease) {
//         switch(event->key) {
//             case InputKeyBack:
//                 with_view_model(
//                     instance->view,
//                     FlipBipScene2Model * model,
//                     {
//                         UNUSED(model);
//                         //flipbip_stop_all_sound(instance->context);
//                         instance->callback(FlipBipCustomEventScene2Back, instance->context);
//                         flipbip_play_long_bump(instance->context);
//                     },
//                     true);
//                 break;
//             case InputKeyUp:
//                 with_view_model(
//                     instance->view,
//                     FlipBipScene2Model * model,
//                     {
//                         model->screen_text = 6;
//                         flipbip_play_bad_bump(instance->context);
//                         //flipbip_stop_all_sound(instance->context);
//                         flipbip_led_set_rgb(instance->context, 255, 0, 255);
//                     },
//                     true);
//                 break;
//             case InputKeyDown:
//                 with_view_model(
//                     instance->view,
//                     FlipBipScene2Model * model,
//                     {
//                         model->screen_text = 7;
//                         flipbip_play_bad_bump(instance->context);
//                         //flipbip_stop_all_sound(instance->context);
//                         flipbip_led_set_rgb(instance->context, 255, 255, 0);
//                     },
//                     true);
//                 break;
//             case InputKeyLeft:
//                 with_view_model(
//                     instance->view,
//                     FlipBipScene2Model * model,
//                     {
//                         model->screen_text = 8;
//                         flipbip_play_bad_bump(instance->context);
//                         //flipbip_stop_all_sound(instance->context);
//                         flipbip_led_set_rgb(instance->context, 0, 255, 255);
//                     },
//                     true);
//                 break;
//             case InputKeyRight:
//                 with_view_model(
//                     instance->view,
//                     FlipBipScene2Model * model,
//                     {
//                         model->screen_text = 9;
//                         flipbip_play_bad_bump(instance->context);
//                         //flipbip_stop_all_sound(instance->context);
//                         flipbip_led_set_rgb(instance->context, 255, 0, 0);
//                     },
//                     true);
//                 break;
//             case InputKeyOk:
//                 with_view_model(
//                     instance->view,
//                     FlipBipScene2Model * model,
//                     {
//                         model->screen_text = 10;
//                         flipbip_play_bad_bump(instance->context);
//                         //flipbip_stop_all_sound(instance->context);
//                         flipbip_led_set_rgb(instance->context, 255, 255, 255);
//                     },
//                     true);
//                 break;
//             case InputKeyMAX:
//                 break;
//         }
//     } else if (event->type == InputTypePress) {
//          switch(event->key) {
//             case InputKeyUp:
//                 with_view_model(
//                     instance->view,
//                     FlipBipScene2Model * model,
//                     {
//                         model->screen_text = 1;
//                         flipbip_play_happy_bump(instance->context);
//                         //flipbip_play_input_sound(instance->context);
//                     },
//                     true);
//                 break;
//             case InputKeyDown:
//                 with_view_model(
//                     instance->view,
//                     FlipBipScene2Model * model,
//                     {
//                         model->screen_text = 2;
//                         flipbip_play_happy_bump(instance->context);
//                         //flipbip_play_input_sound(instance->context);
//                     },
//                     true);
//                 break;
//             case InputKeyLeft:
//                 with_view_model(
//                     instance->view,
//                     FlipBipScene2Model * model,
//                     {
//                         model->screen_text = 3;
//                         flipbip_play_happy_bump(instance->context);
//                         //flipbip_play_input_sound(instance->context);
//                     },
//                     true);
//                 break;
//             case InputKeyRight:
//                 with_view_model(
//                     instance->view,
//                     FlipBipScene2Model * model,
//                     {
//                         model->screen_text = 4;
//                         flipbip_play_happy_bump(instance->context);
//                         //flipbip_play_input_sound(instance->context);
//                     },
//                     true);
//                 break;
//             case InputKeyOk:
//                 with_view_model(
//                     instance->view,
//                     FlipBipScene2Model * model,
//                     {
//                         model->screen_text = 5;
//                         flipbip_play_happy_bump(instance->context);
//                         //flipbip_play_input_sound(instance->context);
//                     },
//                     true);
//                 break;
//             case InputKeyBack:
//             case InputKeyMAX:
//                 break;
//         }
//     }

//     return true;
// }

// void flipbip_scene_2_exit(void* context) {
//     furi_assert(context);
//     //FlipBip* app = context;
//     //flipbip_stop_all_sound(app);
//     //flipbip_led_reset(app);
// }

// void flipbip_scene_2_enter(void* context) {
//     furi_assert(context);
//     DOLPHIN_DEED(DolphinDeedPluginStart);
// }

// FlipBipScene2* flipbip_scene_2_alloc() {
//     FlipBipScene2* instance = malloc(sizeof(FlipBipScene2));
//     instance->view = view_alloc();
//     view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(FlipBipScene2Model));
//     view_set_context(instance->view, instance);
//     view_set_draw_callback(instance->view, (ViewDrawCallback)flipbip_scene_2_draw);
//     view_set_input_callback(instance->view, flipbip_scene_2_input);
//     //view_set_enter_callback(instance->view, flipbip_scene_2_enter);
//     view_set_exit_callback(instance->view, flipbip_scene_2_exit);

//     with_view_model(
//         instance->view,
//         FlipBipScene2Model * model,
//         {
//             flipbip_scene_2_model_init(model);
//         },
//         true);

//     return instance;
// }

// void flipbip_scene_2_free(FlipBipScene2* instance) {
//     furi_assert(instance);

//     view_free(instance->view);
//     free(instance);
// }

// View* flipbip_scene_2_get_view(FlipBipScene2* instance) {
//     furi_assert(instance);

//     return instance->view;
// }
