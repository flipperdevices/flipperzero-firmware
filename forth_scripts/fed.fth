16384 Constant FILE_BUFFER_SIZE
FILE_BUFFER_SIZE allocate throw Constant FILE_BUFFER
FILE_BUFFER FILE_BUFFER_SIZE + Constant FILE_BUFFER_END
Variable FILE_LENGTH
0 FILE_LENGTH !

4096 Constant CUT_BUFFER_SIZE
CUT_BUFFER_SIZE allocate throw Constant CUT_BUFFER
CUT_BUFFER CUT_BUFFER_SIZE + Constant CUT_BUFFER_END
Variable CUT_BUFFER_LENGTH
0 CUT_BUFFER_LENGTH !

256 Constant CMD_BUFFER_SIZE
CMD_BUFFER_SIZE allocate throw Constant CMD_BUFFER

10 Constant CHAR_NL
44 Constant CHAR_COMMA
32 Constant CHAR_SPACE
100 Constant CHAR_d
101 Constant CHAR_e
105 Constant CHAR_i
106 Constant CHAR_j
112 Constant CHAR_p
113 Constant CHAR_q
119 Constant CHAR_w
120 Constant CHAR_x
121 Constant CHAR_y

: calc-new-rem-addr ( u_str addr_start -- addr_rem_new )
	+
;

: calc-new-file-length ( u_str u_rem addr_start -- u_length )
	FILE_BUFFER - + +
	dup FILE_BUFFER_SIZE > if
		drop FILE_BUFFER_SIZE
	then
;

: calc-line-addr ( u_l -- addr_l )
	0 FILE_BUFFER FILE_LENGTH @ + FILE_BUFFER ?do
		2dup = if
			2drop i unloop exit
		then

		i c@ CHAR_NL = if 
			1 +
		then
	loop
	2drop FILE_BUFFER FILE_LENGTH @ +
;

: calc-line-coords ( u_l1 u_l2 -- addr_l1 addr_l2 )
	swap calc-line-addr swap
	calc-line-addr
;

: truncating-move ( addr_from addr_to u_count -- )
	over FILE_BUFFER_END >= if
		2drop drop exit
	then
	2dup + FILE_BUFFER_END > if
		2dup + FILE_BUFFER_END - -
	then
	move
;

: insert-str ( addr_str u_str addr_start addr_rem u_rem -- )
	2over 2over swap drop swap calc-new-file-length FILE_LENGTH !
	2over calc-new-rem-addr swap truncating-move
	swap truncating-move
;

: insert-lines ( addr_str u_str u_l1 u_l2 -- )
	calc-line-coords
	dup FILE_BUFFER -
	FILE_LENGTH @ swap -
	insert-str
;

: delete-lines ( u_l1 u_l2 -- )
	0 0 2swap insert-lines
;

: join-lines ( u_l1 u_l2 -- )
	calc-line-coords
	swap ?do
		i c@ CHAR_NL = if
			0 0 i i 1 +
			dup FILE_BUFFER -
			FILE_LENGTH @ swap -
			insert-str
		then
	loop
;

: print-lines ( u_l1 u_l2 -- )
	calc-line-coords
	over - cr type
;

: calc-cut-buffer-length ( u_str -- u_length )
	CUT_BUFFER_SIZE over < if
		drop CUT_BUFFER_SIZE
	then
;

: yank-into-cut-buffer ( u_l1 u_l2 -- )
	calc-line-coords
	over - calc-cut-buffer-length
	dup CUT_BUFFER_LENGTH !
	CUT_BUFFER swap move
;

: paste-from-cut-buffer ( u_l1 u_l2 -- )
	CUT_BUFFER CUT_BUFFER_LENGTH @ 2swap
	insert-lines
;

: append-nl-to-cut-buffer ( -- )
	CUT_BUFFER CUT_BUFFER_LENGTH @ +
	dup CUT_BUFFER_END >= if
		drop exit
	then
	CHAR_NL swap c!
	CUT_BUFFER_LENGTH @ 1 + CUT_BUFFER_LENGTH !
;

: append-to-cut-buffer ( -- f )
	CUT_BUFFER CUT_BUFFER_LENGTH @ +
	dup CUT_BUFFER_END >= if
		drop 0 exit
	then
	dup CUT_BUFFER_SIZE CUT_BUFFER_LENGTH @ -
	accept
	2dup s" ." compare 0 = if
		2drop 0 exit
	then
	swap drop
	CUT_BUFFER_LENGTH @ + CUT_BUFFER_LENGTH !
	append-nl-to-cut-buffer
	1
;

: read-into-cut-buffer ( -- )
	0 CUT_BUFFER_LENGTH !
	begin
		cr
		append-to-cut-buffer while
	repeat
;

: read-file-into-buffer ( addr u -- )
	r/o open-file throw
	dup
	FILE_BUFFER FILE_BUFFER_SIZE rot
	read-file throw
	FILE_LENGTH !
	close-file throw
;

: write-buffer-into-file ( addr u -- )
	w/o open-file throw
	dup
	FILE_BUFFER FILE_LENGTH @ rot
	write-file throw
	close-file throw
;

: adv-string ( addr u -- addr u )
	1 -
	swap 1 + swap
;

: get-line-number ( addr u -- u addr u )
	dup >r
	0 0 2swap >number
	r> over = if
		-1 throw
	then
	dup 0 = if
		-1 throw
	then
	rot drop
;

: parse-and-exec-lines-cmd ( addr u -- )
	get-line-number
	over c@ CHAR_COMMA <> if
		-1 throw
	then
	adv-string
	get-line-number
	1 <> if
		-1 throw
	then
	c@ rot rot
	2dup > if
		-1 throw
	then
	rot
	case
		CHAR_d of
			2dup delete-lines
		endof
		CHAR_i of
			read-into-cut-buffer
			2dup paste-from-cut-buffer
		endof
		CHAR_j of
			2dup join-lines
		endof
		CHAR_p of
			2dup print-lines
		endof
		CHAR_x of
			2dup paste-from-cut-buffer
		endof
		CHAR_y of
			2dup yank-into-cut-buffer
		endof
		-1 throw
	endcase
	2drop
;

: preparse-cmd-with-str-param ( addr u -- addr u )
	dup 3 < if
		-1 throw
	then
	adv-string
	over c@ CHAR_SPACE <> if
		-1 throw
	then
	adv-string
;

: parse-and-exec-e-cmd ( addr u -- )
	preparse-cmd-with-str-param
	read-file-into-buffer
;

: parse-and-exec-w-cmd ( addr u -- )
	preparse-cmd-with-str-param
	write-buffer-into-file
;

: parse-and-exec-cmd ( addr u -- f )
	dup 0 = if
		-1 throw
	then
	over c@
	dup CHAR_q = if
		drop
		dup 1 <> if
			-1 throw
		then
		2drop 0 exit
	then
	case
		CHAR_e of
			2dup parse-and-exec-e-cmd
		endof
		CHAR_w of
			2dup parse-and-exec-w-cmd
		endof
		dup 2over rot drop
		parse-and-exec-lines-cmd
	endcase
	2drop 1
;

: print-file-buf ( -- )
	FILE_BUFFER FILE_LENGTH @ cr type cr
;

: print-cut-buf ( -- )
	CUT_BUFFER CUT_BUFFER_LENGTH @ cr type cr
;

: fed-loop ( -- )
	begin
		cr
		CMD_BUFFER dup CMD_BUFFER_SIZE accept
		['] parse-and-exec-cmd catch if
			2drop
			cr s" ?" type
			1
		then
		while
	repeat
;

: fed-cleanup ( -- )
	FILE_BUFFER free throw
	CUT_BUFFER free throw
	CMD_BUFFER free throw
;

: fed ( -- )
	fed-loop
	fed-cleanup
;

fed
