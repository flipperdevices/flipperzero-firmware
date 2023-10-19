/*!
 *  @file flipper-xremote/views/xremote_common_view.h
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Common view and canvas functionality shared between the pages.
 */

#pragma once

#include <furi.h>
#include <gui/view.h>
#include <gui/elements.h>

#include <notification/notification.h>
#include <notification/notification_messages.h>

#include <infrared_transmit.h>
#include <xc_icons.h>

#include "../infrared/infrared_remote.h"

#define XREMOTE_BUTTON_COUNT 26
#define XREMOTE_NAME_MAX 16

#define XREMOTE_COMMAND_POWER "Power"
#define XREMOTE_COMMAND_SETUP "Setup"
#define XREMOTE_COMMAND_INPUT "Input"
#define XREMOTE_COMMAND_MENU "Menu"
#define XREMOTE_COMMAND_LIST "List"
#define XREMOTE_COMMAND_INFO "Info"
#define XREMOTE_COMMAND_BACK "Back"
#define XREMOTE_COMMAND_OK "Ok"
#define XREMOTE_COMMAND_UP "Up"
#define XREMOTE_COMMAND_DOWN "Down"
#define XREMOTE_COMMAND_LEFT "Left"
#define XREMOTE_COMMAND_RIGHT "Right"
#define XREMOTE_COMMAND_JUMP_FORWARD "Next"
#define XREMOTE_COMMAND_JUMP_BACKWARD "Prev"
#define XREMOTE_COMMAND_FAST_FORWARD "Fast_fo"
#define XREMOTE_COMMAND_FAST_BACKWARD "Fast_ba"
#define XREMOTE_COMMAND_PLAY_PAUSE "Play_pa"
#define XREMOTE_COMMAND_PAUSE "Pause"
#define XREMOTE_COMMAND_PLAY "Play"
#define XREMOTE_COMMAND_STOP "Stop"
#define XREMOTE_COMMAND_MUTE "Mute"
#define XREMOTE_COMMAND_MODE "Mode"
#define XREMOTE_COMMAND_VOL_UP "Vol_up"
#define XREMOTE_COMMAND_VOL_DOWN "Vol_dn"
#define XREMOTE_COMMAND_NEXT_CHAN "Ch_next"
#define XREMOTE_COMMAND_PREV_CHAN "Ch_prev"

typedef enum {
    XRemoteEventReserved = 200,
    XRemoteEventSignalReceived,
    XRemoteEventSignalFinish,
    XRemoteEventSignalSave,
    XRemoteEventSignalRetry,
    XRemoteEventSignalSend,
    XRemoteEventSignalSkip,
    XRemoteEventSignalAskExit,
    XRemoteEventSignalExit
} XRemoteEvent;

typedef enum {
    /* Navigation */
    XRemoteIconOk,
    XRemoteIconEnter,
    XRemoteIconBack,
    XRemoteIconArrowUp,
    XRemoteIconArrowDown,
    XRemoteIconArrowLeft,
    XRemoteIconArrowRight,

    /* Playback */
    XRemoteIconPlay,
    XRemoteIconPause,
    XRemoteIconStop,
    XRemoteIconPlayPause,
    XRemoteIconFastForward,
    XRemoteIconFastBackward,
    XRemoteIconJumpForward,
    XRemoteIconJumpBackward
} XRemoteIcon;

typedef struct {
    void* context;
    bool ok_pressed;
    bool back_pressed;
    bool up_pressed;
    bool down_pressed;
    bool left_pressed;
    bool right_pressed;
    bool hold;
} XRemoteViewModel;

typedef enum {
    XRemoteViewNone,
    XRemoteViewSignal,
    XRemoteViewTextInput,
    XRemoteViewDialogExit,

    /* Main page */
    XRemoteViewSubmenu,
    XRemoteViewLearn,
    XRemoteViewSaved,
    XRemoteViewAnalyzer,
    XRemoteViewSettings,
    XRemoteViewAbout,

    /* Remote app */
    XRemoteViewIRSubmenu,
    XRemoteViewIRGeneral,
    XRemoteViewIRControl,
    XRemoteViewIRPlayback,
    XRemoteViewIRNavigation,
    XRemoteViewIRCustomPage,
    XRemoteViewIRCustomEditPage,
    XRemoteViewIRAllButtons
} XRemoteViewID;

typedef struct XRemoteView XRemoteView;
typedef void (*XRemoteClearCallback)(void* context);
typedef void (*XRemoteViewDrawFunction)(Canvas*, XRemoteViewModel*);

typedef XRemoteView* (*XRemoteViewAllocator)(void* app_ctx);
typedef XRemoteView* (*XRemoteViewAllocator2)(void* app_ctx, void* model_ctx);

const char* xremote_button_get_name(int index);
int xremote_button_get_index(const char* name);

void xremote_canvas_draw_header(Canvas* canvas, ViewOrientation orient, const char* section);
void xremote_canvas_draw_exit_footer(Canvas* canvas, ViewOrientation orient, const char* text);

void xremote_canvas_draw_icon(Canvas* canvas, uint8_t x, uint8_t y, XRemoteIcon icon);
void xremote_canvas_draw_button(
    Canvas* canvas,
    bool pressed,
    uint8_t x,
    uint8_t y,
    XRemoteIcon icon);
void xremote_canvas_draw_button_png(
    Canvas* canvas,
    bool pressed,
    uint8_t x,
    uint8_t y,
    const Icon* icon);
void xremote_canvas_draw_button_wide(
    Canvas* canvas,
    bool pressed,
    uint8_t x,
    uint8_t y,
    const char* text,
    XRemoteIcon icon);
void xremote_canvas_draw_button_size(
    Canvas* canvas,
    bool pressed,
    uint8_t x,
    uint8_t y,
    uint8_t xy,
    char* text,
    XRemoteIcon icon);
void xremote_canvas_draw_frame(
    Canvas* canvas,
    bool pressed,
    uint8_t x,
    uint8_t y,
    uint8_t xl,
    const char* text);

XRemoteView*
    xremote_view_alloc(void* app_ctx, ViewInputCallback input_cb, ViewDrawCallback draw_cb);
XRemoteView* xremote_view_alloc_empty();
void xremote_view_free(XRemoteView* rview);

InfraredRemoteButton* xremote_view_get_button_by_name(XRemoteView* rview, const char* name);
bool xremote_view_press_button(XRemoteView* rview, InfraredRemoteButton* button);
bool xremote_view_send_ir_msg_by_name(XRemoteView* rview, const char* name);

void xremote_view_model_context_set(XRemoteView* rview, void* model_ctx);
void xremote_view_clear_context(XRemoteView* rview);

void xremote_view_set_app_context(XRemoteView* rview, void* app_ctx);
void xremote_view_set_context(XRemoteView* rview, void* context, XRemoteClearCallback on_clear);
void xremote_view_set_view(XRemoteView* rview, View* view);

void* xremote_view_get_app_context(XRemoteView* rview);
void* xremote_view_get_context(XRemoteView* rview);
View* xremote_view_get_view(XRemoteView* rview);
