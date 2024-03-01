# Minesweeper Implementation for the Flipper Zero.

## Added features unique to this implementation:
-	Enable board verifier for board generation to ensure unambiguous boards!
-	Set board width and height
-	Set difficulty
-	A number of different button presses as shortcuts for game actions (see How To Play) 


## How To Play
1. Up/Down/Left/Right Buttons to move around
	- These buttons can be held down to keep moving in a direction
2. Center OK Button to attempt opening up a tile
	- Press OK on a tile to open it up
	- Hold OK on a cleared space with a number to clear all surrounding tiles (correct number of flags must be set around it)
3. Hold Back Button on a tile to toggle marking it with a flag
4. Hold Back Button on a cleared space to jump to one of the closest tiles (this can help find last tiles on a larger board)
5. Press Back Button to access the settings menu where you can do the following:
	- Change board width
	- Change board height
	- Change difficulty
	- Ensure Solvable (**Important!**)- This option will enable the board verifier for board generation and can significantly increase wait times for generating a board. Currently this may lock up the view port on the loading animation, but it should eventually resolve and return to the game.

## IMPORTANT NOTICE:
The way I set the board up leaves the corners as safe starting positions!

In addition to this, with the "Ensure Solvable" option set to true, the board will always be solvable from 0,0! Without Ensure Solvable enabled in the settings the mine placement for the board generation is randomized and there is no guarantee that the game with be solvable without any guesses.
