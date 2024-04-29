[![ufbt Build and Release action tag](https://github.com/squee72564/F0_Minesweeper_Fap/actions/workflows/build.yml/badge.svg)](https://github.com/squee72564/F0_Minesweeper_Fap/actions/workflows/build.yml)

# Minesweeper Implementation for the Flipper Zero.
Hello!

![Mine Sweeper Example Gameplay Gif](https://github.com/squee72564/F0_Minesweeper_Fap/blob/main/img/github_images/MineSweeperGameplay.gif)
## Added features unique to this implementation:
-	Enable board verifier for board generation to ensure unambiguous boards!
-	Set board width and height
-	Set difficulty
-	A number of different button presses as shortcuts for game actions (see How To Play) 

## Installation
The app is also now available for download on the [Flipper Lab](https://lab.flipper.net/apps/minesweeper_redux) website for easy access.

If you want to manually build and install follow the steps below: 
* Clone this repo to your computer:
	`git clone https://github.com/squee72564/F0_Minesweeper_Fap.git`

* I would recommend using the [uFBT (micro Flipper Build Tool)](https://github.com/flipperdevices/flipperzero-ufbt) to build the application.
	- It allows you to compile into .fap files on the SD card as opposed to re-flashing the entire firmware with the application.
	- With uFBT installed you can go into the root directory of this project after cloning and type
	`ufbt launch` to compile and launch the app on your Flipper Zero

## How To Play
- Up/Down/Left/Right Buttons to move around
	- These buttons can be held down to keep moving in a direction
- Center OK Button to attempt opening up a tile
	- Press OK on a tile to open it up
	- Hold OK on a cleared space with a number to clear all surrounding tiles (correct number of flags must be set around it)
- Hold Back Button on a tile to toggle marking it with a flag
- Hold Back Button on a cleared space to jump to one of the closest tiles (this can help find last tiles on a larger board)
- Press Back Button to access the settings menu where you can do the following:
	- Change board width
	- Change board height
	- Change difficulty
	- Ensure Solvable (**Important!**) : This option will enable the board verifier for board generation and can significantly increase wait times for generating a board. Currently this may lock up the view port on the loading animation, but it should eventually resolve and return to the game.
	- Enable Feedback : This option toggles the haptic and sound feedback for the game.
    - Enable Wrap : This option toggles wrapping movement to the other side of the board when you move across the edge boundary.

## IMPORTANT NOTICE:
The way I set the board up leaves the corners as safe starting positions!

In addition to this, with the "Ensure Solvable" option set to true, the board will always be solvable from 0,0! Without "Ensure Solvable" enabled in the settings there is no guarantee that the game will be solvable without any guesses.

## Application Structure
The following is the application structure with a breakdown of each folder:
-  **[F0_Minesweeper_Fap/](https://github.com/squee72564/F0_Minesweeper_Fap)**
	- [github/workflows/](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/.github/workflows)
		- Github workflows folder
	- [img/](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/img), [assets/](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/assets) and [docs/](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/docs)
		-  Contains custom image assets, documentation, changelog, and other miscellaneous items
	- [scenes/](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/scenes)
		- Contains `minesweeper_scene.c`, `minesweeper_scene.h`, and `minesweeper_scene_config.h` files to automate creation of scene FP arrays and Scene enumeration types. View the `.README` in this folder for more information on adding new scenes.
		-  Contains the enter/event/exit functions for each scene used in a .c file
	- [views/](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/views)
		-  Contains the .c and .h files for any custom views used.
	- [helpers/](https://github.com/squee72564/F0_Minesweeper_Fap/tree/main/helpers)
		- Helper files for functions related to hardware access and control
	- minesweeper.c / minesweeper.h
		- Main Mine Sweeper App .c and .h file
	- application.fam
		- [Build system manifest file](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/AppManifests.md#application-definition) for the application

Right now the current structure of the application is heavily inspired from [leedave's](https://github.com/leedave/flipper-zero-fap-boilerplate) boilerplate repo.
