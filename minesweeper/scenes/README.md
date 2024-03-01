# IMPORTANT INFO

This scene directory has 3 important files in addition to other scene specific files:

1. [minesweeper_scene.c](https://github.com/squee72564/F0_Minesweeper_Fap/blob/main/scenes/minesweeper_scene.c)
	- File generates on_enter/exit/event handler fp arrays for Scene Manager and Initializes Scene Handlers configuration structure
2. [minesweeper_scene.h](https://github.com/squee72564/F0_Minesweeper_Fap/blob/main/scenes/minesweeper_scene.h)
	- File generates on_enter/exit/event handler function declarations for all scenes
	- Generates Enum for all scenes 
3. [minesweeper_scene_config.h](https://github.com/squee72564/F0_Minesweeper_Fap/blob/main/scenes/minesweeper_scene_config.h)
	- Macro definition of ADD_SCENE() for each scene used.
		- When adding new scenes you need to add a new macro definition to this file, along with the appropriate .c file for the definitions of the on_event/enter/exit functions for the scene.