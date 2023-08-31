433920000 Constant DEFAULT_FREQ
1024 chars Constant RX_BUFFER_SIZE
1024 chars Constant TX_BUFFER_SIZE
256 chars Constant INPUT_BUFFER_SIZE
50 Constant TICK_INTERVAL
500 Constant MSG_COMPLETION_TIMEOUT
4096 Constant RX_TASK_STACK_SIZE
10 Constant MSG_TX_DELAY

13 Constant CHAR_CR
10 Constant CHAR_NL
58 Constant CHAR_COLON

0 cells Constant OFS_RX_TASK
1 cells Constant OFS_FREQ
2 cells Constant OFS_SUBGHZ_WORKER
3 cells Constant OFS_SUBGHZ_DEVICE
4 cells Constant OFS_LAST_TIME_RX_DATA
5 cells Constant OFS_RUNNING
6 cells Constant OFS_RX_BUFFER
6 cells RX_BUFFER_SIZE + Constant OFS_TX_BUFFER
6 cells RX_BUFFER_SIZE + TX_BUFFER_SIZE + Constant CHAT_STRUCT_SIZE

: C_STRLEN ( addr -- u )
	dup ( -- addr addr )
	BEGIN
		dup c@ 0 <> while
			1 chars + ( -- addr addr )
	REPEAT
	swap -
;

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

: WRITE_OUT_TX_BUF ( addr u -- )
	>r
	dup OFS_SUBGHZ_WORKER + @
	swap OFS_TX_BUFFER +
	r>
	SUBGHZ_TX_RX_WORKER_WRITE drop
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

: APPEND_BUF ( addr_str u_str addr_buf -- )
	swap move
;

: CALC_TX_BUF_COPY_SIZE ( u_append u_cur_tx_size -- u_copy_size )
	2dup + TX_BUFFER_SIZE > if
		swap drop
		TX_BUFFER_SIZE swap -
	else
		drop
	then
;

: APPEND_TX_BUF ( addr_str u_str addr_struct u_tx_size -- u_new_tx_size )
	swap OFS_TX_BUFFER + over + >r ( -- addr_str u_str u_tx_size )
	swap over ( -- addr_str u_tx_size u_str u_tx_size )
	CALC_TX_BUF_COPY_SIZE ( -- addr_str u_tx_size u_copy_size )
	swap over + ( -- addr_str u_copy_size u_new_tx_size )
	r> swap >r ( -- addr_str u_copy_size addr_tx_buf )
	APPEND_BUF ( -- )
	r> ( -- u_new_tx_size )
;

: APPEND_CR_TX_BUF ( addr_struct u_tx_size -- u_new_tx_size )
	TX_BUFFER_SIZE over - 2 >= if
		swap OFS_TX_BUFFER + over + ( -- u_tx_size addr_tx_buf )
		CHAR_CR over ! ( -- u_tx_size addr_tx_buf )
		1 + ( -- u_tx_size addr_tx_buf )
		CHAR_NL swap ! ( -- u_tx_size )
		2 +
	else
		swap drop
	then
;

: TX_BUF_START_MSG ( addr_struct -- u_new_tx_size )
	>r
	FURI_HAL_VERSION_GET_NAME_PTR dup C_STRLEN ( -- addr u )
	r> 0 ( -- addr u addr_struct u_cur_tx_size )
	APPEND_TX_BUF ( -- u_new_tx_size )
;

: TX_BUF_JOIN_MSG ( addr_struct -- u_tx_size )
	dup TX_BUF_START_MSG ( -- addr_struct u_tx_size )
	>r >r ( -- )
	s"  joined chat." r> r> over >r ( -- addr_str u_str addr_struct u_tx_size )
	APPEND_TX_BUF ( -- u_tx_size )
	r> swap APPEND_CR_TX_BUF
;

: TX_BUF_LEAVE_MSG ( addr_struct -- u_tx_size )
	dup TX_BUF_START_MSG ( -- addr_struct u_tx_size )
	>r >r ( -- )
	s"  left chat." r> r> over >r ( -- addr_str u_str addr_struct u_tx_size )
	APPEND_TX_BUF ( -- u_tx_size )
	r> swap APPEND_CR_TX_BUF
;

: TX_BUF_MSG ( addr_str u_str addr_struct -- u_tx_size )
	dup TX_BUF_START_MSG ( -- addr_str u_str addr_struct u_tx_size )
	>r >r ( -- addr_str u_str )
	s" : " r> r> over >r ( -- addr_str u_str addr_str2 u_str2 addr_struct u_tx_size )
	APPEND_TX_BUF ( -- addr_str u_str u_tx_size )
	r@ swap ( -- addr_str u_str addr_struct u_tx_size )
	APPEND_TX_BUF ( -- u_tx_size )
	r> swap APPEND_CR_TX_BUF
;

: TX_JOIN_MSG ( addr_struct -- )
	dup TX_BUF_JOIN_MSG
	WRITE_OUT_TX_BUF
	MSG_TX_DELAY FURI_DELAY_MS
;

: TX_LEAVE_MSG ( addr_struct -- )
	dup TX_BUF_LEAVE_MSG
	WRITE_OUT_TX_BUF
	MSG_TX_DELAY FURI_DELAY_MS
;

: TX_MSG ( addr_struct addr_str u_str -- )
	rot >r r@
	TX_BUF_MSG
	r> swap
	WRITE_OUT_TX_BUF
	MSG_TX_DELAY FURI_DELAY_MS
;

: DISPLAY_AND_TX_MSG ( addr_struct addr_str u_str -- )
	cr
	FURI_HAL_VERSION_GET_NAME_PTR dup C_STRLEN type
	CHAR_COLON emit space
	2dup type cr
	TX_MSG
;

: RX_STRUCT_ALLOC ( freq device -- addr )
	CHAT_STRUCT_SIZE allocate throw >r
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

: CHAT_TX_LOOP ( addr_struct -- )
	INPUT_BUFFER_SIZE allocate throw INPUT_BUFFER_SIZE ( -- addr_struct addr_input u_input )
	BEGIN
		drop dup INPUT_BUFFER_SIZE accept ( -- addr_struct addr_input u_input )
		2dup s" exit" compare WHILE ( -- addr_struct addr_input u_input )
			dup 2over rot DISPLAY_AND_TX_MSG ( -- addr_struct addr_input u_input )
	REPEAT
	drop free throw drop
;

: CHAT ( freq -- )
	CHAT_RX_INIT
	dup TX_JOIN_MSG
	dup CHAT_TX_LOOP
	dup TX_LEAVE_MSG
	CHAT_RX_TERM
;

s" Starting chat on default frequency. Leave by typing 'exit'." type cr
DEFAULT_FREQ CHAT
