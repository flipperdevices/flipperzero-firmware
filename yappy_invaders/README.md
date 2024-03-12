# Yappy Invaders
Yappy Invaders is a space invaders clone designed specifically for the [Flipper Zero](https://flipperzero.one/) device. It combines the classic gameplay of space invaders with the unique hardware capabilities of Flipper Zero, creating an engaging experience for users.

## Features
Classic space invader mechanics: move your ship left and right to shoot and kill the Yapps.
Slowly increasing difficulty with each wave of enemies.
Simple and intuitive controls using Flipper Zero's built-in buttons.
Score tracking for each game session.
Game Over screen with the option to restart.

## Installation
To install Yappy Invaders on your Flipper Zero, follow these steps:

1. Clone this repository to your local machine. ```git clone https://github.com/dagnazty/Yappy_Invaders```
2. Navigate to the app directory within the cloned repository: ```cd Yappy_Invaders```
3. Ensure ufbt is installed.
   Windows: ```py -m pip install --upgrade ufbt```
   Mac/Linux: ```python3 -m pip install --upgrade ufbt```
4. Build the application using the Flipper Zero SDK: ```ufbt build```
5. To install the application onto your Flipper Zero, use the ufbt tool with the following command: ```ufbt install```
6. Look for Yappy Invaders in Apps/Games.

![1p](https://raw.githubusercontent.com/dagnazty/Yappy_Invaders/main/images/1.png)

## Controls
Left/Right Buttons: Move your ship left or right.

OK Button: Shoot projectiles at enemies.

Back Button: Quit the game.

## Gameplay
You control a spaceship at the bottom of the screen, moving left and right to dodge enemy fire and shooting projectiles to destroy enemies. Each hit on an enemy increases your score. The game's difficulty increases as you destroy all enemies and progress to the next wave. The game ends when an enemy ship reaches the bottom of the screen, displaying a game over message and a prompt to restart the game.

![2p](https://raw.githubusercontent.com/dagnazty/Yappy_Invaders/main/images/2.png)

![3p](https://raw.githubusercontent.com/dagnazty/Yappy_Invaders/main/images/3.png)

## Contributing
Contributions to Yappy Invaders are welcome! If you have suggestions for improvements or encounter any issues, please open an issue or submit a pull request.

## License
Yappy Invaders is released under the MIT License. See the LICENSE file for more details.
