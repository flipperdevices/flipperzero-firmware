//----------------------------------------------------------------------------- ----------------------------------------
// Includes
//

// System libs
#include  <stdlib.h>       // malloc
#include  <stdint.h>       // uint32_t
#include  <stdarg.h>       // __VA_ARGS__

// FlipperZero libs
#include  <furi.h>         // Core API
#include  <gui/gui.h>      // GUI (screen/keyboard) API
#include  <input/input.h>  // GUI Input extensions

// There is no 'getter' for gui->canvas
// ...this is dirty/risky, but quicker and (a LOT) easier than getting changes committed to the official API
// ...essentially, we are exposing private variables [yes, C has private variables - qv. "typedef struct Gui Gui;"]
// --> applications/gui/
#include  <gui/gui_i.h>

// The FlipperZero Settings->System menu allows you to set the logging level at RUN-time
// LOG_LEVEL lets you limit it at COMPILE-time
//    1.  None
//    2.  Errors      : ERROR -> FURI_LOG_E
//    3.  Warnings    : WARN  -> FURI_LOG_W
//    4.  Information : INFO  -> FURI_LOG_I
//    5.  Debug       : DEBUG -> FURI_LOG_D
//    6.  Trace       : TRACE -> FURI_LOG_T
// Also provides ENTER and LEAVE -> TRACE
#define  LOG_LEVEL  6
#include  "bc_logging.h"

// Local headers
#include  "bc_demo.h"  // Various enums and struct declarations
#include  "err.h"      // Error numbers & messages

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//   OOOOO    // SSSSS       CCCCC  AAA  L     L     BBBB   AAA   CCCC K   K  SSSSS
//   O   O   /// S           C     A   A L     L     B   B A   A C     K  K   S
//   O   O  ///  SSSSS       C     AAAAA L     L     BBBB  AAAAA C     KKK    SSSSS
//   O   O ///       S       C     A   A L     L     B   B A   A C     K  K       S
//   OOOOO //    SSSSS       CCCCC A   A LLLLL LLLLL BBBB  A   A  CCCC K   K  SSSSS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//+============================================================================ ========================================
// OS Callback : Timer tick
// We register this function to be called when the OS signals a timer 'tick' event
//
static
void  cbTimer (FuriMessageQueue* queue)
{
	// Every function has an ENTER at the top, and a LEAVE before it returns
	// ...these generate code flow messages if TRACE level messaging in enabled
	ENTER;

	// Sanity checks
	// ...furi_assert() is removed at /compile-time/ unless FURI_DEBUG is specified
	// ...if you want checks to be performed on release code, use: furi_check()
	// --> furi/core/check.h
	furi_assert(queue);

	// Put a "tick" event message on the message queue
	// furi_message_queue_put  |  furi/core/message_queue.*
	eventMsg_t  message = {.id = EVID_TICK};
	furi_message_queue_put(queue, &message, 0);

	LEAVE;
	return;
}

//+============================================================================ ========================================
// OS Callback : Keypress
// We register this function to be called when the OS detects a keypress
//
static
void  cbInput (InputEvent* event,  FuriMessageQueue* queue)
{
	ENTER;
	furi_assert(queue);
	furi_assert(event);

	// Put an "input" event message on the message queue
	eventMsg_t  message = {.id = EVID_KEY,  .input = *event};
	furi_message_queue_put(queue, &message, FuriWaitForever);

	LEAVE;
	return;
}

//+============================================================================ ========================================
// OS Callback : Draw request
// We register this function to be called when the OS requests that the screen is redrawn
//
// We actually instruct the OS to perform this request, after we update the interface
// I guess it's possible that this instruction may able be issued by other threads !?
//
static
void  cbDraw (Canvas* const canvas,  void* ctx)
{
	ENTER;
	furi_assert(canvas);
	furi_assert(ctx);

	// We will need access to the plugin state variables
	state_t*  state;  // state pointer

	// Try to acquire the mutex for the plugin state variables, timeout = 25mS
	// We (obviously) do not (yet) have access to the state variables,
	//   so the timeout value must be specified at compile-time
	//
	// On success we will receive a pointer to the plugin state variables
	// ...On fail, we simply (have to) bail out
	//
	// acquire_mutex()  |  furi/core/valuemutex.*
	//   -->  furi_mutex_acquire()  |  furi/core/mutex.*
	//   -->  xSemaphoreTake()  |  lib/FreeRTOS-Kernel/include/semphr.h
	if ( !(state = (state_t*)acquire_mutex((ValueMutex*)ctx, 25)) )  return ;

	// Border around the edge of the screen
	// top-left is {0,0}, [standard] screen is 128x64 {WxH}
	canvas_draw_frame(canvas, 0, 0, state->cnvW, state->cnvH);

	switch (state->animID) {
		case ANIM_TEXT:
			// There are several built-in fonts available
			//   {FontPrimary, FontSecondary, FontKeyboard, FontBigNumbers}
			// --> applications/gui/canvas.c
			canvas_set_font(canvas, state->font);

			// Alignments can be {Left, Right, Top, Bottom, Center}
			// --> applications/gui/canvas.h
			canvas_draw_str_aligned(canvas, state->x, state->y, AlignLeft, AlignTop, state->text);
			break;

		case ANIM_BALL:
			canvas_draw_circle(canvas, state->x, state->y, state->ballR);
			break;

		default:
			canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignCenter, "?");
			break;
	}

	// Release the mutex
	release_mutex((ValueMutex*)ctx, state);

	LEAVE;
	return;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//   EEEEE V   V EEEEE N   N TTTTT       H   H  AAA  N   N DDDD  L     EEEEE RRRR  SSSSS
//   E     V   V E     NN  N   T         H   H A   A NN  N  D  D L     E     R   R S
//   EEE    V V  EEE   N N N   T         HHHHH AAAAA N N N  D  D L     EEE   RRRR  SSSSS
//   E      V V  E     N  NN   T         H   H A   A N  NN  D  D L     E     R R       S
//   EEEE    V   EEEEE N   N   T         H   H A   A N   N DDDD  LLLLL EEEEE R  R  SSSSS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// If we want the code in a sane order, we need to "prototype" [forward declare] these functions
static void  animateSet (animID_t,  state_t* const,  const Gui*) ;
static void  animateEn  (state_t*,  bool on) ;
static void  animate    (state_t*) ;
static bool  move       (state_t*,  dir_t) ;

//+============================================================================ ========================================
// Event Handler : Tick
// This (demo) just triggers an animation event
//
static
void  evTick (state_t* state)
{
	ENTER;
	furi_assert(state);

	animate(state);

	LEAVE;
	return;
}

//+============================================================================ ========================================
// Handle a key press event
//
static inline
bool  evKey (eventMsg_t* msg,  state_t* state, Gui* gui)
{
	ENTER;
	furi_assert(msg);
	furi_assert(state);
	furi_assert(gui);

	bool  run = true;  // assume keep running

	switch (msg->input.type) {
		case InputTypeShort:   // Short   - after InputTypeRelease within INPUT_LONG_PRESS interval
			if (msg->input.key == InputKeyOk)  animateEn(state, !state->animate) ;
			break;

		case InputTypeLong:    // Long    - after INPUT_LONG_PRESS interval, asynch to InputTypeRelease
			if (msg->input.key == InputKeyOk)  animateSet((state->animID +1) % ANIM_CNT, state, gui) ;
			break;

		case InputTypePress:   // Press   - after debounce
			switch (msg->input.key) {
   				// Stop animations before moving
				case InputKeyUp:
				case InputKeyDown:
				case InputKeyLeft:
				case InputKeyRight:
					if (state->animate)  animateEn(state, false) ;
					break;

				// Ignore keys
				case InputKeyOk:
				case InputKeyBack:
					break;

				// Unknown key
				default:
					if (msg->input.type == InputTypePress)
						WARN("%s : Unknown key [%d]", __func__, msg->input.key);
					break;
			}
			// Now treat the keystroke like a repeat
			__attribute__ ((fallthrough));

		case InputTypeRepeat:  // Repeat  - with INPUT_REPEATE_PRESS period after InputTypeLong event
			switch (msg->input.key) {
   				// Move the logo around
				case InputKeyUp:     move(state, DIR_U);  break ;
				case InputKeyDown:   move(state, DIR_D);  break ;
				case InputKeyLeft:   move(state, DIR_L);  break ;
				case InputKeyRight:  move(state, DIR_R);  break ;

				// Ignore keys
				case InputKeyOk:
				case InputKeyBack:
				// Unknown key
				default:
					break;
			}
			break;

		case InputTypeRelease: // Release - after debounce
			if (msg->input.key == InputKeyBack)  run = false ;  // Signal the plugin to exit
			break;

		default:
			WARN("%s : Unknown event [%d]", __func__, msg->input.type);
			break;
	}

	LEAVE;
	return run;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//   MM MM  OOO  V   V EEEEE MM MM EEEEE N   N TTTTT
//   M M M O   O V   V E     M M M E     NN  N   T
//   M M M O   O  V V  EEE   M M M EEE   N N N   T
//   M   M O   O  V V  E     M   M E     N  NN   T
//   M   M  OOO    V   EEEEE M   M EEEEE N   N   T
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//+============================================================================ ========================================
// Move the x,y anchor position
//
static
bool  move (state_t* state,  dir_t dir)
{
	ENTER;
	furi_assert(state);

	bool  rv   = true;

	// Up/Down movement
	switch (dir & (DIR_U | DIR_D)) {
		case DIR_U:
			if (state->y > state->minY)  state->y-- ;
			else                         rv = false ;
			break;
		case DIR_D:
			if (state->y < state->maxY)  state->y++ ;
			else                         rv = false ;
			break;
		default:
			break;
	}

	// Left/Right movement
	switch (dir & (DIR_L | DIR_R)) {
		case DIR_L:
			if (state->x > state->minX)  state->x-- ;
			else                         rv = false ;
			break;
		case DIR_R:
			if (state->x < state->maxX)  state->x++ ;
			else                         rv = false ;
			break;
		default:
			break;
	}

	LEAVE;
	return rv;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    AAA  N   N IIIII MM MM  AAA  TTTTT IIIII  OOO  N   N SSSSS
//   A   A NN  N   I   M M M A   A   T     I   O   O NN  N S
//   AAAAA N N N   I   M M M AAAAA   T     I   O   O N N N SSSSS
//   A   A N  NN   I   M   M A   A   T     I   O   O N  NN     S
//   A   A N   N IIIII M   M A   A   T   IIIII  OOO  N   N SSSSS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//+============================================================================ ========================================
// Select and configure an animation
//
static
void  animateSet (animID_t id,  state_t* const state,  const Gui* gui)
{
	ENTER;
	furi_assert(state);
	furi_assert(gui);
	furi_assert(id < ANIM_CNT);

	switch ((state->animID = id)) {
		default:
		case ANIM_TEXT:
			//! use of "gui->canvas" is a hack!
			canvas_set_font(gui->canvas, state->font);
			// I allow "descenders" to duck off the bottom of the screen ..."-1" is the border
			state->minX  = 1;
			state->maxX  = (state->cnvW -1) -canvas_string_width(gui->canvas, state->text);
			state->minY  = 1;
			state->maxY  = (state->cnvH -1) -canvas_get_font_params(gui->canvas, state->font)->height;
			break;

		case ANIM_BALL:
			state->minX  = state->ballR +1;
			state->maxX  = (state->cnvW -1) -state->ballR -1;
			state->minY  = state->ballR +1;
			state->maxY  = (state->cnvH -1) -state->ballR -1;
			break;
	}

	LEAVE;
	return;
}

//+============================================================================ ========================================
// Enable/Disable animation
//
static
void  animateEn (state_t* state,  bool on)
{
	ENTER;
	furi_assert(state);

	// ENable animations
	if (on) {
		if (state->animate) {
			WARN(errs[WARN_ANIM_START]);
		} else {
			// Set the timer to fire at 'fps' times/second
			if (furi_timer_start(state->timer, state->timerHz/state->fps) != FuriStatusOk) {
				ERROR(errs[ERR_TIMER_START]);
			} else {
				state->animate = true;
				state->dir     = ((dir_t[]){DIR_UL, DIR_UR, DIR_DL, DIR_DR})[rand() & 0x3];
			}
		}

	// DISable animations
	} else {
		if (!state->animate) {
			WARN(errs[WARN_ANIM_STOP]);
		} else {
			// Stop the timer
			if (furi_timer_stop(state->timer) != FuriStatusOk) {
				ERROR(errs[ERR_TIMER_STOP]);
			} else {
				state->animate = false;
			}
		}
	}

	LEAVE;
	return;
}

//+============================================================================ ========================================
// Animate the logo
//
static
void  animate (state_t* state)
{
	ENTER;
	furi_assert(state);

	// Animations are disabled ?
	if (!state->animate)  return ;

	// UP/DOWN requested?
	if (state->dir & DIR_U) {
		// change direction at the edge of the screen
		if (!move(state, DIR_U))  state->dir ^= DIR_U | DIR_D ;
	} else if (state->dir & DIR_D) {
		if (!move(state, DIR_D))  state->dir ^= DIR_U | DIR_D ;
	}

	// LEFT/RIGHT requested?
	if (state->dir & DIR_L) {
		if (!move(state, DIR_L))  state->dir ^= DIR_L | DIR_R ;
	} else if (state->dir & DIR_R) {
		if (!move(state, DIR_R))  state->dir ^= DIR_L | DIR_R ;
	}

	LEAVE;
	return;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//   SSSSS TTTTT  AAA  TTTTT EEEEE       V   V  AAA  RRRR  IIIII  AAA  BBBB  L     EEEEE SSSSS
//   S       T   A   A   T   E           V   V A   A R   R   I   A   A B   B L     E     S
//   SSSSS   T   AAAAA   T   EEE          V V  AAAAA RRRR    I   AAAAA BBBB  L     EEE   SSSSS
//       S   T   A   A   T   E            V V  A   A R R     I   A   A B   B L     E         S
//   SSSSS   T   A   A   T   EEEEE         V   A   A R  R  IIIII A   A BBBB  LLLLL EEEEE SSSSS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//+============================================================================ ========================================
// Initialise plugin state variables
//
static inline
bool  stateInit (state_t* const state,  const Gui* gui)
{
	ENTER;
	furi_assert(state);
	furi_assert(gui);

	bool rv = true;  // assume success

	// Default values
	state->x          = 1;
	state->y          = 1;

	state->animate    = false;
	state->dir        = DIR_NONE;
	state->animID     = ANIM_TEXT;

	state->timer      = NULL;
	state->timerHz    = furi_kernel_get_tick_frequency();
	state->fps        = 12;

	state->font       = FontPrimary;
	if (!(state->text = strdup("BlueChip"))) {
		ERROR(errs[ERR_MALLOC_TEXT]);
		rv = false;
		goto bail;
	}

	//! use of "gui->canvas" is a hack!
	state->cnvW       = canvas_width(gui->canvas);
	state->cnvH       = canvas_height(gui->canvas);

	state->ballR      = 5;  // radius
	animateSet(state->animID, state, gui);

	// Seed the PRNG
	// CYCCNT --> lib/STM32CubeWB/Drivers/CMSIS/Include/core_cm7.h
	srand(DWT->CYCCNT);

bail:
	LEAVE;
	return rv;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//   MM MM  AAA  IIIII N   N
//   M M M A   A   I   NN  N
//   M M M AAAAA   I   N N N
//   M   M A   A   I   N  NN
//   M   M A   A IIIII N   N
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//+============================================================================ ========================================
// Plugin entry point
//
int32_t  bc_demo (void)
{
	ENTER;

	// ===== Variables =====

	// Setup may fail in any number of amusing ways
	err_t              error   = 0;  // assume success

	// The GUI (input & output) is accessed via a "viewport"
	// --> applications/gui/gui.*
	Gui*               gui     = NULL;
	// --> applications/gui/view_port.*
	ViewPort*          vpp     = NULL;

	// The plugin has "state" variables
	// ...these [seeminglyn!?] need to be stored on the heap
	// --> local
	state_t*           state   = NULL;  // PluginStatePointer

	// The OS is threaded, so we will protect the state variables with a mutex
	// --> furi/core/valuemutex.*
	ValueMutex         mutex   = {0};

	// This plugin will register two callbacks
	// The key reading ("input") and timer ("tick") callback functions will put keystrokes in a local message queue
	// --> furi/core/message_queue.h
	FuriMessageQueue*  queue   = NULL;
	const uint32_t     queueSz = 8;     // maximum messages in queue

	// The queue will contain plugin event-messages
	// --> local
	eventMsg_t         msg     = {0};

	// Enable the main loop
	bool               run     = 1;

	INFO("BEGIN");

	// ===== Message queue =====

	// 1. Create a message queue (for up to 8 (keyboard) event messages)
	if ( !(queue = furi_message_queue_alloc(queueSz, sizeof(msg))) ) {
		ERROR(errs[(error = ERR_MALLOC_QUEUE)]);
		goto bail;
	}

	// ===== Create GUI Interface =====

	// 2. Create a GUI interface
	if ( !(gui = furi_record_open("gui")) ) {
		ERROR(errs[(error = ERR_NO_GUI)]);
		goto bail;
	}

	// ===== Plugin state variables =====

	// 3. Allocate space on the heap for the plugin state variables
	if ( !(state = malloc(sizeof(state_t))) ) {
		ERROR(errs[(error = ERR_MALLOC_STATE)]);
		goto bail;
	}

	// 4. Initialise the plugin state variables
	if (!stateInit(state, gui)) {
		// error message(s) is/are output by stateInit()
		error = 15;
		goto bail;
	}

	// 5. Create a mutex for (reading/writing) the plugin state variables
	if (!init_mutex(&mutex, state, sizeof(state))) {
		ERROR(errs[(error = ERR_NO_MUTEX)]);
		goto bail;
	}

	// ===== Viewport =====

	// 6. Allocate space on the heap for the viewport
	if ( !(vpp = view_port_alloc()) ) {
		ERROR(errs[(error = ERR_MALLOC_VIEW)]);
		goto bail;
	}

	// 7a. Register a callback for input events
	view_port_input_callback_set(vpp, cbInput, queue);

	// 7b. Register a callback for draw events
	view_port_draw_callback_set(vpp, cbDraw, &mutex);

	// ===== Start GUI Interface =====

	// 8. Attach the viewport to the GUI
	gui_add_view_port(gui, vpp, GuiLayerFullscreen);

	// ===== Animation timer =====

	// 9. Allocate a timer
	if ( !(state->timer = furi_timer_alloc(cbTimer, FuriTimerTypePeriodic, queue)) ) {
		ERROR(errs[(error = ERR_NO_TIMER)]);
		goto bail;
	}

	INFO("INITIALISED");

	// ==================== Main event loop ====================

	if (run)  do {
		// Try to read a message from the queue
		// Our run-loop does not poll and is not "busy"
		//   but there is no "do not timeout"/"wait for message"
		//   so we need to use a large timeout and ignore timeout messages
		// --> furi/core/base.h
		FuriStatus  status = FuriStatusErrorTimeout;
		while ((status = furi_message_queue_get(queue, &msg, 60000)) == FuriStatusErrorTimeout) ;

		// Read failed
		if (status != FuriStatusOk) {
			switch (status) {
				case FuriStatusErrorTimeout:    DEBUG(errs[       DEBUG_QUEUE_TIMEOUT]);    break ;
				case FuriStatusError:           ERROR(errs[(error = ERR_QUEUE_RTOS)]);      goto bail ;
				case FuriStatusErrorResource:   ERROR(errs[(error = ERR_QUEUE_RESOURCE)]);  goto bail ;
				case FuriStatusErrorParameter:  ERROR(errs[(error = ERR_QUEUE_BADPRM)]);    goto bail ;
				case FuriStatusErrorNoMemory:   ERROR(errs[(error = ERR_QUEUE_NOMEM)]);     goto bail ;
				case FuriStatusErrorISR:        ERROR(errs[(error = ERR_QUEUE_ISR)]);       goto bail ;
				default:                        ERROR(errs[(error = ERR_QUEUE_UNK)]);       goto bail ;
			}

		// Read successful
		} else {
			// *** Try to lock the plugin state variables ***
			if ( !(state = (state_t*)acquire_mutex_block(&mutex)) ) {
				ERROR(errs[(error = ERR_MUTEX_BLOCK)]);
				goto bail;
			}

			// *** Handle events ***
			switch (msg.id) {
				// ------------------------------------------------------------
				// Timer events
				case EVID_TICK:
					evTick(state);  // Animation runs every "tick"
					break;

				// ------------------------------------------------------------
				// Key events
				case EVID_KEY:
					run = evKey(&msg, state, gui);
					break;

				// ------------------------------------------------------------
				// Unknown event
				default:
					WARN("Unknown message.ID [%d]", msg.id);
					break;
			}

			// Update the GUI screen via the viewport
			view_port_update(vpp);

			// *** Try to release the plugin state variables ***
			if ( !release_mutex(&mutex, state) ) {
				ERROR(errs[(error = ERR_MUTEX_RELEASE)]);
				goto bail;
			}
		} // if (ok)
	} while (run);

	// ===== Game Over =====

	INFO("USER EXIT");

// Oh no! A horrible scary "goto" label!
// Quick! Call the Spanish Inquisition! ...Bring the comfy chair!
bail:

	// I would rather have done this with the "atexit()" stack,
	//   that way, the Setup & Teardown sequences keep themselves in synch
	// However ...this is not "main()", and, as such, it does not "exit()"
	//   ...and I am not implementing a "atPluginExit()" here

	// 9. Stop the timer
	if (state->timer) {
		(void)furi_timer_stop(state->timer);
		furi_timer_free(state->timer);
		state->timer = NULL;
	}

	// 8. Detach the viewport
	gui_remove_view_port(gui, vpp);

	// 7. No need to unreqgister the callbacks
	//    ...they will go when the viewport is destroyed

	// 6. Destroy the viewport
	if (vpp) {
		view_port_enabled_set(vpp, false);
		view_port_free(vpp);
		vpp = NULL;
	}

	// 5. Free the mutex
	if (mutex.mutex) {
		delete_mutex(&mutex);
		mutex.mutex = NULL;
	}

	// 4. Free up state pointer(s)
	if (state->text) {
		free(state->text);
		state->text = NULL;
	}

	// 3. Free the plugin state variables
	if (state) {
		free(state);
		state = NULL;
	}

	// 2. Close the GUI
	furi_record_close("gui");

	// 1. Destroy the message queue
	if (queue) {
		furi_message_queue_free(queue);
		queue = NULL;
	}

	INFO("CLEAN EXIT ... Exit code: %d", error);
	LEAVE;
	return (int32_t)(error ? 255 : 0);  // It *seems* that the options are 0 for success, or 255 for failure
}
