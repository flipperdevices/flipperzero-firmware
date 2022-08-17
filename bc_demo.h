#ifndef  BC_DEMO_H_
#define  BC_DEMO_H_

//----------------------------------------------------------------------------- ----------------------------------------
// A list of event IDs handled by this plugin
//
typedef
	enum eventID {
		EVID_NONE,
		EVID_UNKNOWN,

		// A full list of events can be found with:  `grep -r --color  "void.*set_.*_callback"  applications/gui/*`
		// ...A free gift to you from the makers of well written code that conforms to a good coding standard
		EVID_KEY,
		EVID_TICK,
	}
eventID_t;

//----------------------------------------------------------------------------- ----------------------------------------
// Animation details
//

// A list of available animations
typedef
	enum animID {
		ANIM_TEXT,
		ANIM_BALL,
		ANIM_CNT  // MUST be last
	}
animID_t;

// Movement directions
typedef
	enum dir {
		DIR_NONE = 0x00,
		DIR_U    = 0x01,
		DIR_D    = 0x02,
		DIR_L    = 0x04,
		DIR_R    = 0x08,
		DIR_UL   = DIR_U | DIR_L,
		DIR_UR   = DIR_U | DIR_R,
		DIR_DL   = DIR_D | DIR_L,
		DIR_DR   = DIR_D | DIR_R,
	}
dir_t;

//----------------------------------------------------------------------------- ----------------------------------------
// An item in the event message-queue
//
typedef
	struct eventMsg {
		eventID_t   id;     // --> local
		InputEvent  input;  // --> applications/input/input.h
	}
eventMsg_t;

//----------------------------------------------------------------------------- ----------------------------------------
// State variables for this plugin
// An instance of this is allocated on the heap, and the pointer is passed back to the OS
// Access to this memory is controlled by mutex
//
typedef 
	struct state {
		// Animation is a classic bouncing icon
		bool         animate;  // true -> animation enabled
		animID_t     animID;   // which logo
		dir_t        dir;      // animation direction

		// We will need a timer to run the animation
	    FuriTimer*   timer;    // the timer
		uint32_t     timerHz;  // system ticks per second
		int          fps;      // animation frames-per-second

		// x,y coords of icon
		int          x;        // current x coord
		int          y;        // current y coord

		Font         font;     // Font for...
		char*        text;     //   Bouncing text

		int          ballR;    // Ball radius

		// movement limits
		int          cnvW;     // canvas width
		int          minX;     // min x coord
		int          maxX;     // max x coord

		int          cnvH;     // canvas height
		int          minY;     // min x coord
		int          maxY;     // max y coord
	}
state_t;

#endif //BC_DEMO_H_
