# Minesweeper Implementation for the Flipper Zero.
Hello!

This is my first flipper zero application and this repo will be a learning process in general.

If you have any tips for structuring the application and best practices for managing the views/scenes
I am always welcome to feedback!

Right now the current structure of the application is heavily inspired from [leedave's](https://github.com/leedave/flipper-zero-fap-boilerplate) boilerplate repo.

## Installation
1. Clone this repo to your computer ie:
	`git clone https://github.com/squee72564/F0_Minesweeper_Fap.git`
3. I would recommend using the [uFBT (micro Flipper Build Tool)](https://github.com/flipperdevices/flipperzero-ufbt) to build the application.
	- It allows you to compile into .fap files on the SD card as opposed to re-flashing the entire firmware with the application.
	- With uFBT installed you can go into the root directory of this project after cloning and type
	`ufbt launch` to compile and launch the app on your Flipper Zero
4. Profit


The following is the application structure with a breakdown of each folder.
## Application Structure
-  **[F0_Minesweeper_Fap](https://github.com/squee72564/F0_Minesweeper_Fap)**
	- [github/workflows](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/.github/workflows)
		- Github workflows folder
	- [dist](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/dist)
		-  Contains .fap file for game when compiled
	- [images](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/images)
		-  Contains all custom image assets used for game
	- [scenes](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/scenes)
		- Contains `minesweeper_scene.c`, `minesweeper_scene.h`, and `minesweeper_scene_config.h` files to automate creation of scene FP arrays and Scene enumeration types. View the `.README` in this folder for more information on adding new scenes.
		-  Contains the enter/event/exit functions for each scene used in a .c file
	- [views](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/views)
		-  Contains the .c and .h files for any custom views used.
		- Right now there is the StartScreen custom view built for the game. 