 ,--------.
(  Biblio  )
 `--------'

Resources
	Meta-list of cool stuff. Includes forks with country restrictions removed
		https://github.com/djsime1/awesome-flipperzero

Local docs
	You really should at least take a peek at the docs in this folder!
		documentation/*

 ,-----------------------.
(  Install & First Build  )
 `-----------------------'

# You may have multiple firmwares in separate folders {official, Unleashed, RogueMaster, ...}
FZROOT=/path/to/flipperZero/

# This is the firmware we are demonstrating (the official branch)
FZTHIS=${FZROOT}/official/

# ...and this is how to retrieve the demo (official) firmware
mkdir -p ${FZTHIS}
cd       ${FZTHIS}
git clone --recursive  https://github.com/flipperdevices/flipperzero-firmware.git  .

# The FIRST run will pull down all required toochains, RTOS, etc.
# ...AND THEN perform the first build of all code
# Do NOT interrupt this process! (it can't recover if a partial zip has been downloaded)
./fbt

 ,----------.
(  Flashing  )
 `----------'

# qv. documentation/fbt.md
./fbt flash_usb


 ,---------.
(  Plugins  )
 `---------'

This readme should exist in a directory called 'bc_demo'

1. Move the bc_demo directory to applications/  [**]

Eg.
	/path/to/FlipperZero/
	+-- official/
	:   +-- applications/            <-- plugins etc.
	:   |   +-- bc_demo/             <-- bc_demo has been added [**]
	:   |   |   +-- README           <--   this file
	:   |   |   +-- application.fam  <--   plugin ID file for OS
	:   |   |   +-- bc_demo.c        <--   excessively documented plugin code
	:   |   |
	:   |   +-- gui/                 <-- The GUI library
	:   |   |
	:   |   +-- meta/                <-- System menu config file
	:   |   :   +-- application.fam  <-- ...
	:   |
	:   +-- documentation/           <-- Really, RTFMs, or at least skim them!  Not kidding!
	:   |   +-- KeyCombo.md
	:   |   +-- OTA.md
	:   |   +-- fbt.md
	:   |   :
	:   |   
	:   +-- furi/                    <-- Flipper API
	:   :   +-- core/
	:   :

2. Add the plugin to the menu system

# Get plugin's "appid"
grep  appid  ${FZTHIS}/applications/bc_demo/application.fam

# Add it to the menu somewhere
# ...if you can't work out how, probably best give up now :/
edit ${FZTHIS}/applications/meta/application.fam

3. Rebuild the firmware ...Sadly touching the meta/ folder triggers a rebuild of ALL appliations
./fbt


 ,---------.
(  bc_demo  )
 `---------'

The demo code is *EXTENSIVELY* documented

--> application.fam
	# This holds the details required by the toolchain to compile it in (as a plugin)

--> err.h
	# Keep error numbers and strings tied together in a friendly way

--> err.c
	# Local-Global storage for the error strings

--> bc_logging.h
	# Disable logging calls at compile-time
	# Helper macros {ERROR, WARN, INFO, DEBUG, TRACE, ENTER, LEAVE}
	# Retrieves 'appName' from 'err.h'

--> bc_demo.h
	- eventID_t  : Event IDs
	- animID_t   : Animation IDs
	- dir_t      : Movement directions
	- eventMsg_t : Event message struct
	- state_t    : Plugin state variables

--> bc_demo.c
	+ bc_demo
		-> stateInit()
		== cbRender()
		== cbInput()
		-> evTick()
		-> evKey()

		# ===== Message queue =====
		# 1. Create a message queue (for up to 8 (keyboard) event messages)
		# ===== Create GUI Interface =====
		# 2. Create a GUI interface
		# ===== Plugin state variables =====
		# 3. Allocate space on the heap for the plugin state variables
		# 4. Initialise the plugin state variables  -> stateInit()
		# 5. Create a mutex for the plugin state variables
		# ===== Viewport =====
		# 6. Allocate space on the heap for the viewport
		# 7a. Register a callback for draw events   == cbRender()
		# 7b. Register a callback for input events  == cbInput()
		# ===== Start GUI Interface =====
		# 8. Attach the viewport to the GUI
		# ===== Animation timer =====
		# 9. Allocate a timer

		# ==================== Main event loop ====================
		# LOOP
		# | Get a message from the queue
		# | Lock the plugin state variables
		# | | Handle events:
		# | | +-- Timer Tick -> evTick()
		# | | `-- Keypress   -> evKey() ... may reply "stop running"
		# | | Update the GUI screen via the viewport
		# | UnLock the plugin state variables
		# +---

		# ===== Game Over =====
		# 9. Destroy the timer
		# 8. Detach the viewport
		# 7. No need to unreqgister the callbacks
		#    ...they will go when the viewport is destroyed
		# 6. Destroy the viewport
		# 5. Free the mutex
		# 4. Free up state pointer(s)
		# 3. Free the plugin state variables
		# 2. Close the GUI
		# 1. Destroy the message queue
		# == EXIT ==

	+ stateInit()
		<- bc_demo()

	+ animateSet()
		<- stateInit()
		<- evKey() [Long-OK] <- bc_demo()
		# Activate one of the animations
		# Set screen movement limits

	+ animateEn()
		<- evKey() [Short-OK] <- bc_demo()
		# Start/Stop the interrupt timer

	+ animate()
		<- evTick() <- bc_demo()
		-> move()
		# Perform a single frame of animation -> move()

	+ move()
		<- animate() <- evTick() <- bc_demo()
		<- evKey() [Press|Repeat-{Up,Down,Left,Right}] <- bc_demo()
		# Move the icon in the specified direction

	+ evTick()
		<- bc_demo()
		# Perform Tick events (ie. aniamte)

	+ evKey()
		<- bc_demo()
		# Process keyboard
		#   Short-OK     = start/stop animation       -> animateEn()
		#   Long -OK     = cycle through animations   -> animateSet()
		#   Press-Dir    = stop animation & move icon -> animateEn(), move()
		#   Repeat-Dir   = move icon                  -> move()
		#   Release-Back = signal: exit plugin        -> run=0

	+ cbTimer()
		<- O/S <- bc_demo [9]
		# Put a Tick event on the messaeg queue

	+ cbInput()
		<- O/S <- bc_demo [7a]
		# Put an Key event on the messaeg queue

	+ cbDraw()            [7b]
		<- O/S <- bc_demo
		# (Re-)Draw the whole canvas
