
# Minesweeper Implementation for the Flipper Zero.
Hello!

![Mine Sweeper Start Animation](https://github.com/squee72564/F0_Minesweeper_Fap/blob/main/images/github_images/StartScreenAnimationGif.gif)

This is my first flipper zero application and this repo will be a learning process in general.

Right now the current structure of the application is heavily inspired from [leedave's](https://github.com/leedave/flipper-zero-fap-boilerplate) boilerplate repo.

## Installation
1. Clone this repo to your computer ie:
	`git clone https://github.com/squee72564/F0_Minesweeper_Fap.git`
3. I would recommend using the [uFBT (micro Flipper Build Tool)](https://github.com/flipperdevices/flipperzero-ufbt) to build the application.
	- It allows you to compile into .fap files on the SD card as opposed to re-flashing the entire firmware with the application.
	- With uFBT installed you can go into the root directory of this project after cloning and type
	`ufbt launch` to compile and launch the app on your Flipper Zero
4. Profit

## How To Play
- Up/Down/Left/Right Buttons to move around (can be held to continuously move)
- Center OK Button to attempt opening up a tile
- Hold Back Button on a covered tile to toggle marking it with a flag
- Hold Back Button on a uncovered tile to jump to one of the closest covered tiles (this can help find last covered tiles on a larger board)
- Press Back Button to access the settings menu where you can do the following:
	- Change board width
	- Change board height
	- Change difficulty (still experimenting with values for this)

#### NOTICE:
The way I set the board up leaves the corners as safe starting positions with the top left tile 0,0 always having an unambiguous next move. I opted for this instead of opening up the board on the first tile the user selects.

This is done to help the solver and simplify the game start. 

## Application Structure
The following is the application structure with a breakdown of each folder:
-  **[F0_Minesweeper_Fap/](https://github.com/squee72564/F0_Minesweeper_Fap)**
	- [github/workflows/](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/.github/workflows)
		- Github workflows folder
	- [dist/](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/dist)
		-  Contains .fap file for game when compiled
	- [images/](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/images)
		-  Contains all custom image assets used for game
	- [scenes/](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/scenes)
		- Contains `minesweeper_scene.c`, `minesweeper_scene.h`, and `minesweeper_scene_config.h` files to automate creation of scene FP arrays and Scene enumeration types. View the `.README` in this folder for more information on adding new scenes.
		-  Contains the enter/event/exit functions for each scene used in a .c file
	- [views/](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/views)
		-  Contains the .c and .h files for any custom views used.
		- Right now there is the StartScreen custom view built for the game. 
	- minesweeper.c / minesweeper.h
		- Main Mine Sweeper App .c and .h file
	- application.fam
		- [Build system manifest file](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/AppManifests.md#application-definition) for the application

## TODO
- Implement mine sweeper solver for board generation
	- Using a solver to check the generated board can ensure that it can be solved without any guesses. I think this would be more fair to the player and would ensure more interesting boards where you are not just guessing.
	- The one downside is that this will introduce much more overhead for the board setup as the game will need to continuously generate and attempt to solve maps until it gets a valid one.
	- I may have it attempt to generate and solve maps for a set time period, and if it cannot do so it will use the next random map, solve it even if it needs to guess perfectly multiple times, and then inform the user that a unambiguous map could not be generated in the set amount of time and that it will take N guesses to solve.