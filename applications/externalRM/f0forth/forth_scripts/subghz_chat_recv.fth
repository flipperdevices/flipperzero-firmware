433920000 Constant DEFAULT_FREQ
1024 chars Constant RX_BUFFER_SIZE
50 Constant TICK_INTERVAL
500 Constant MSG_COMPLETION_TIMEOUT
4096 Constant RX_TASK_STACK_SIZE

0 cells Constant OFS_RX_TASK
1 cells Constant OFS_FREQ
2 cells Constant OFS_SUBGHZ_WORKER
3 cells Constant OFS_SUBGHZ_DEVICE
4 cells Constant OFS_LAST_TIME_RX_DATA
5 cells Constant OFS_RUNNING
6 cells Constant OFS_RX_BUFFER
6 cells RX_BUFFER_SIZE + Constant RX_STRUCT_SIZE

: RX_NOTIF ( -- )
	RECORD_NOTIFICATION FURI_RECORD_OPEN
	dup SEQUENCE_SINGLE_VIBRO NOTIFICATION_MESSAGE
	drop
	RECORD_NOTIFICATION FURI_RECORD_CLOSE
;

: POST_RX ( c-addr u -- )
	TYPE
	RX_NOTIF
;

: HAVE_READ_CB ( addr -- )
	FURI_GET_TICK swap OFS_LAST_TIME_RX_DATA + !
;

: READ_INTO_RX_BUF ( addr -- u )
	dup OFS_SUBGHZ_WORKER + @
	swap OFS_RX_BUFFER +
	RX_BUFFER_SIZE
	SUBGHZ_TX_RX_WORKER_READ
;

: READ_AND_TYPE ( addr -- )
	dup READ_INTO_RX_BUF
	swap OFS_RX_BUFFER + swap
	POST_RX
;

: WORKER_AVAIL ( addr -- u )
	OFS_SUBGHZ_WORKER + @
	SUBGHZ_TX_RX_WORKER_AVAILABLE
;

: GET_LAST_TIME_RX_DATA ( addr -- u )
	OFS_LAST_TIME_RX_DATA + @
;

: DO_RX ( addr -- )
	BEGIN
		dup >r ( -- addr )
		r@ WORKER_AVAIL dup 0 > ( -- addr u f )
		swap RX_BUFFER_SIZE < ( -- addr f f )
		FURI_GET_TICK r> GET_LAST_TIME_RX_DATA - MSG_COMPLETION_TIMEOUT < ( -- addr f f f )
		AND INVERT AND WHILE ( -- addr )
			dup READ_AND_TYPE ( -- addr )
	REPEAT
	drop
;

: RX_TASK ( addr -- n )
	BEGIN
		dup OFS_RUNNING + @ WHILE
			dup DO_RX
			TICK_INTERVAL FURI_DELAY_MS
	REPEAT
	drop 0
;

: RX_STRUCT_ALLOC ( freq device -- addr )
	RX_STRUCT_SIZE allocate throw >r
	r@ OFS_SUBGHZ_DEVICE + !
	r@ OFS_FREQ + !
	0 r@ OFS_LAST_TIME_RX_DATA + !
	1 r@ OFS_RUNNING + !
	0 RX_TASK_STACK_SIZE ['] RX_TASK r@ FURI_THREAD_ALLOC_EX r@ OFS_RX_TASK + !
	SUBGHZ_TX_RX_WORKER_ALLOC r@ OFS_SUBGHZ_WORKER + !
	r@ OFS_SUBGHZ_WORKER + @ ['] HAVE_READ_CB r@ SUBGHZ_TX_RX_WORKER_SET_CALLBACK
	r>
;

: RX_STRUCT_FREE ( addr -- )
	dup OFS_RX_TASK + @ FURI_THREAD_FREE
	dup OFS_SUBGHZ_WORKER + @ SUBGHZ_TX_RX_WORKER_FREE
	free throw
;

: START_RX_TASK ( addr -- )
	OFS_RX_TASK + @ FURI_THREAD_START
;

: STOP_RX_TASK ( addr -- )
	0 over OFS_RUNNING + !
	OFS_RX_TASK + @ FURI_THREAD_JOIN drop
;

: START_WORKER ( addr -- )
	>r
	r@ OFS_SUBGHZ_WORKER + @
	r@ OFS_SUBGHZ_DEVICE + @
	r> OFS_FREQ + @
	SUBGHZ_TX_RX_WORKER_START drop
;

: STOP_WORKER ( addr -- )
	dup OFS_SUBGHZ_WORKER + @
	SUBGHZ_TX_RX_WORKER_IS_RUNNING if
		OFS_SUBGHZ_WORKER + @ SUBGHZ_TX_RX_WORKER_STOP
	else
		drop
	then
;

: DEV_INIT ( -- device )
	SUBGHZ_DEVICES_INIT
	SUBGHZ_DEVICE_CC1101_INT_NAME SUBGHZ_DEVICES_GET_BY_NAME
;

: DEV_DEINIT ( -- )
	SUBGHZ_DEVICES_DEINIT
;

: CHAT_RX_INIT ( freq -- addr )
	DEV_INIT
	RX_STRUCT_ALLOC
	dup START_WORKER
	dup START_RX_TASK
;

: CHAT_RX_TERM ( addr -- )
	dup STOP_RX_TASK
	dup STOP_WORKER
	DEV_DEINIT
	RX_STRUCT_FREE
;

\ How to start the chat receiver:
\ - Include this file:
\   s" <path to file>" included
\ - Specify a frequency and call CHAT_RX_INIT:
\   DEFAULT_FREQ CHAT_RX_INIT
\ - Wait for chat messages. They should be displayed.
\ - To terminate the receiver call CHAT_RX_TERM on the address left by CHAT_RX_INIT:
\   CHAT_RX_TERM
