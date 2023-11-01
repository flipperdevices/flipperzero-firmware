/***************************************************************
** Call Custom Functions for pForth
**
** Create a file similar to this and compile it into pForth
** by setting -DPF_USER_CUSTOM="mycustom.c"
**
** Using this, you could, for example, call X11 from Forth.
** See "pf_cglue.c" for more information.
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** Permission to use, copy, modify, and/or distribute this
** software for any purpose with or without fee is hereby granted.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
** THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
** CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
** FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
** CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
** OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**
***************************************************************/

#include "pforth.h"
#include "pf_types.h"
#define PF_USER_FILEIO "pf_fileio_f0.h"
#include "pf_io.h"
#include "pf_guts.h"
#include "pf_cglue.h"

#include <furi.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <lib/subghz/subghz_tx_rx_worker.h>
#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>

#define DEFAULT_RETURN_DEPTH (512)
#define DEFAULT_USER_DEPTH (512)

struct thread_callback_ctx {
	ExecToken XT;
	void *actual_ctx;
};

static void *ThreadContextAlloc(ExecToken XT, void *actual_ctx)
{
	struct thread_callback_ctx *cb_ctx = malloc(sizeof(struct
				thread_callback_ctx));

	cb_ctx->XT = XT;
	cb_ctx->actual_ctx = actual_ctx;

	return cb_ctx;
}

static int32_t ThreadCallbackDispatcher(void* context)
{
	struct thread_callback_ctx *cb_ctx = context;

	pfTaskData_t *gCurrentTask = pfCreateTask(DEFAULT_USER_DEPTH,
			DEFAULT_RETURN_DEPTH);

	PUSH_DATA_STACK(cb_ctx->actual_ctx);
	pfCatch(cb_ctx->XT, gCurrentTask);
	cell_t ret = POP_DATA_STACK;

	pfDeleteTask(gCurrentTask);

	free(cb_ctx);
	return ret;
}

static void HaveReadCallbackDispatcher(void* context)
{
	struct thread_callback_ctx *cb_ctx = context;

	pfTaskData_t *gCurrentTask = pfCreateTask(DEFAULT_USER_DEPTH,
			DEFAULT_RETURN_DEPTH);

	PUSH_DATA_STACK(cb_ctx->actual_ctx);
	pfCatch(cb_ctx->XT, gCurrentTask);

	pfDeleteTask(gCurrentTask);

	free(cb_ctx);
}

/****************************************************************
** Step 1: Put your own special glue routines here
**     or link them in from another file or library.
****************************************************************/
static char *RecordNotificationWrapper(void)
{
	return RECORD_NOTIFICATION;
}

static void *SequenceSingleVibroWrapper(void)
{
	return (void*) &sequence_single_vibro;
}

static void *SequenceDoubleVibroWrapper(void)
{
	return (void*) &sequence_double_vibro;
}

static void *SequenceSuccessWrapper(void)
{
	return (void*) &sequence_success;
}

static void *SequenceErrorWrapper(void)
{
	return (void*) &sequence_error;
}

static void *SequenceAudiovisualAlertWrapper(void)
{
	return (void*) &sequence_audiovisual_alert;
}

static FuriThread *FuriThreadAllocExWrapper(const char *name, uint32_t
		stack_size, FuriThreadCallback callback, void *context)
{
	void *ctx = ThreadContextAlloc((ExecToken) callback, context);
	FuriThreadCallback cb = ThreadCallbackDispatcher;

	return furi_thread_alloc_ex(name, stack_size, cb, ctx);
}

static void SubghzTxRxWorkerSetCallbackHaveReadWrapper(SubGhzTxRxWorker*
		instance, SubGhzTxRxWorkerCallbackHaveRead callback, void*
		context)
{
	void *ctx = ThreadContextAlloc((ExecToken) callback, context);
	SubGhzTxRxWorkerCallbackHaveRead cb = HaveReadCallbackDispatcher;

	subghz_tx_rx_worker_set_callback_have_read(instance, cb, ctx);
}

static const char *SubghzDeviceCC1101IntNameWrapper(void)
{
	return SUBGHZ_DEVICE_CC1101_INT_NAME;
}

/****************************************************************
** Step 2: Create CustomFunctionTable.
**     Do not change the name of CustomFunctionTable!
**     It is used by the pForth kernel.
****************************************************************/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
CFunc0 CustomFunctionTable[] =
{
	(CFunc0) furi_delay_ms,
	(CFunc0) furi_get_tick,
	(CFunc0) furi_hal_version_get_name_ptr,
	(CFunc0) furi_record_open,
	(CFunc0) furi_record_close,
	(CFunc0) RecordNotificationWrapper,
	(CFunc0) notification_message,
	(CFunc0) SequenceSingleVibroWrapper,
	(CFunc0) SequenceDoubleVibroWrapper,
	(CFunc0) SequenceSuccessWrapper,
	(CFunc0) SequenceErrorWrapper,
	(CFunc0) SequenceAudiovisualAlertWrapper,
	(CFunc0) FuriThreadAllocExWrapper,
	(CFunc0) furi_thread_free,
	(CFunc0) furi_thread_start,
	(CFunc0) furi_thread_join,
	(CFunc0) subghz_tx_rx_worker_available,
	(CFunc0) subghz_tx_rx_worker_read,
	(CFunc0) subghz_tx_rx_worker_write,
	(CFunc0) subghz_tx_rx_worker_alloc,
	(CFunc0) subghz_tx_rx_worker_free,
	(CFunc0) subghz_tx_rx_worker_start,
	(CFunc0) subghz_tx_rx_worker_stop,
	(CFunc0) subghz_tx_rx_worker_is_running,
	(CFunc0) SubghzTxRxWorkerSetCallbackHaveReadWrapper,
	(CFunc0) subghz_devices_init,
	(CFunc0) subghz_devices_deinit,
	(CFunc0) subghz_devices_get_by_name,
	(CFunc0) SubghzDeviceCC1101IntNameWrapper,
};
#pragma GCC diagnostic pop

/****************************************************************
** Step 3: Add custom functions to the dictionary.
**     Do not change the name of CompileCustomFunctions!
**     It is called by the pForth kernel.
****************************************************************/

Err CompileCustomFunctions(void)
{
	Err err;
	int i = 0;
/* Compile Forth words that call your custom functions.
** Make sure order of functions matches that in LoadCustomFunctionTable().
** Parameters are: Name in UPPER CASE, Function, Index, Mode, NumParams
*/
	err = CreateGlueToC( "FURI_DELAY_MS", i++, C_RETURNS_VOID, 1 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "FURI_GET_TICK", i++, C_RETURNS_VALUE, 0 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "FURI_HAL_VERSION_GET_NAME_PTR", i++, C_RETURNS_VALUE, 0 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "FURI_RECORD_OPEN", i++, C_RETURNS_VALUE, 1 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "FURI_RECORD_CLOSE", i++, C_RETURNS_VOID, 1 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "RECORD_NOTIFICATION", i++, C_RETURNS_VALUE, 0 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "NOTIFICATION_MESSAGE", i++, C_RETURNS_VOID, 2 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SEQUENCE_SINGLE_VIBRO", i++, C_RETURNS_VALUE, 0 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SEQUENCE_DOUBLE_VIBRO", i++, C_RETURNS_VALUE, 0 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SEQUENCE_SUCCESS", i++, C_RETURNS_VALUE, 0 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SEQUENCE_ERROR", i++, C_RETURNS_VALUE, 0 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SEQUENCE_AUDIOVISUAL_ALERT", i++, C_RETURNS_VALUE, 0 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "FURI_THREAD_ALLOC_EX", i++, C_RETURNS_VALUE, 4 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "FURI_THREAD_FREE", i++, C_RETURNS_VOID, 1 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "FURI_THREAD_START", i++, C_RETURNS_VOID, 1 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "FURI_THREAD_JOIN", i++, C_RETURNS_VALUE, 1 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SUBGHZ_TX_RX_WORKER_AVAILABLE", i++, C_RETURNS_VALUE, 1 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SUBGHZ_TX_RX_WORKER_READ", i++, C_RETURNS_VALUE, 3 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SUBGHZ_TX_RX_WORKER_WRITE", i++, C_RETURNS_VALUE, 3 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SUBGHZ_TX_RX_WORKER_ALLOC", i++, C_RETURNS_VALUE, 0 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SUBGHZ_TX_RX_WORKER_FREE", i++, C_RETURNS_VOID, 1 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SUBGHZ_TX_RX_WORKER_START", i++, C_RETURNS_VALUE, 3 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SUBGHZ_TX_RX_WORKER_STOP", i++, C_RETURNS_VOID, 1 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SUBGHZ_TX_RX_WORKER_IS_RUNNING", i++, C_RETURNS_VALUE, 1 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SUBGHZ_TX_RX_WORKER_SET_CALLBACK", i++, C_RETURNS_VOID, 3 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SUBGHZ_DEVICES_INIT", i++, C_RETURNS_VOID, 0 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SUBGHZ_DEVICES_DEINIT", i++, C_RETURNS_VOID, 0 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SUBGHZ_DEVICES_GET_BY_NAME", i++, C_RETURNS_VALUE, 1 );
	if( err < 0 ) return err;
	err = CreateGlueToC( "SUBGHZ_DEVICE_CC1101_INT_NAME", i++, C_RETURNS_VALUE, 0 );
	if( err < 0 ) return err;

	return 0;
}

/****************************************************************
** Step 4: Recompile using compiler option PF_USER_CUSTOM
**         and link with your code.
**         Then rebuild the Forth using "pforth -i system.fth"
**         Test:   10 Ctest0 ( should print message then '11' )
****************************************************************/
