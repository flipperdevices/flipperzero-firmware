# Rock Papper Scissors
![Flipper Zero Game](./prebuilt/win.png)

## Working builds

This multi-player application relies on a SubGHz API that was recently changed.  Older firmware will get a build error, due to not supporting the new API definitions.

It should build successfully on the following versions:
- Official firmware - RC 33.0
- Official firmware - Release 33.0
- Official firmware - DEV 34.1
- Unleashed firmware - DEV 34.0
- RogueMaster firmware - DEV 34.1

If you get error ``unknown type name 'SubGhzDevice'`` trying to build, then you need a newer firmware version.


## Introduction

This is a multi-player game of Rock, Paper, Scissors! In version 1.2, we use the [Flipper Zero](https://flipperzero.one/) Sub-GHz radio to find other players, communicate moves, and exchange contact information.

Watch on [YouTube](https://youtu.be/eGOv6Gbar7I).


## Quick install directions for Flipper Zero running Official Firmware!
I recommend [FlipC.org](https://flipc.org/jamisonderek/flipper-zero-tutorials?branch=main&root=subghz/plugins/rock_paper_scissors&firmware=official) as the easiest way to install the game on your Flipper Zero.

- If you have a different firmware, see the [installation directions](#installation-directions) below.


## Playing the game

These directions assume you are starting at the flipper desktop. If not, please press the back button until you are at the desktop.

- On both Flippers, launch the "Rock Paper Scissors" application.
  - Press the OK button on the flipper to pull up the main menu.
  - Choose "Apps" from the menu.
  - Choose "Games" from the sub-menu.
  - Choose "Rock Paper Scissors"

- On both Flippers, choose "Edit contact info".
  - Enter your Discord name, or Facebook name, or whatever you want to share with other players.
  - Hold the backspace button (top right) to clear the field.
  - Hold a letter button to choose lowercase letters.
  - Use the enter button (bottom right) to save your contact info.

- On Flipper 1, choose "Host game".
  - select a valid frequency. (like "433.92" in US region)
  - choose a game number (like "042")
  - click OK button to start game.

- On Flipper 2, choose "Join game".
  - select the same valid frequency as Flipper 1.
  - "game none" should change to show the game number from Flipper 1 & its name.
  - click OK button to join game.

- Once two players are joined:
  - Press "OK" to send "1". The other player should also press "OK" to send "1" back - Press "OK" to send "2". The other player should also press "OK" to send "2" back - Press "UP" to send a "Rock", or "RIGHT" to send a "Paper", or "DOWN" to send a "Scissors".
  - Rules:
    - Rock beats Scissors.
    - Paper beats Rock.
    - Scissors beat Paper.
    - Two identical items tie.

- Play again screen will show the results of the game.
  - Click "Right" to select "No".
  - Click "Left" to select "Yes" (the default).
  - Click "OK" to answer the prompt.

- Short press the BACK button for the main menu.

- Long press the BACK button to exit the game.


## Status

V1 completed features:
- The game logic is complete (two flippers can play against each other.)
- UI to show Flipper images instead of just debug text.
- Allow changing contact information (discord, Facebook, etc.)
- Allow changing frequency, in case one Flipper is in a restricted region.
- Allow changing game number, in case multiple games at a conference.
- Show games found & let user pick the game to join.
- Receiving a Join game does an ACK (to cause game on joiner to start).
- A join ACK removes it from the list of available games.
- Vibro on button press (valid move).
- "Song" when win/loss/draw.
- Log game results & contact info onto SD card.
- Allow viewing past games/scores.
- Relaxed timing, so Count 1/Count 2/RPS no longer have to be timed tightly.
- Added a "Play again" screen.
- Play again from one Flipper Zero auto-starts/exit other Flipper Zero.


Planed work for V2.0:

- Wrap user content to two lines on Past Games screen.
- Allow for replacement of keyboard layout/keys.
- Refactor the code, so it has less duplication.
- Write tutorial.


Future ideas:

- Add game ending animations.
- Create stand-alone hardware badges that can play the game too.
- Uses Princeton signals for communication.
- Instead of subghz, use GPIO.
- Instead of subghz, use IR.


## Installation Directions

This project is intended to be overlayed on top of an existing firmware repo.

- Clone, Build &amp; Deploy an existing flipper zero firmware repo. See this [tutorial](/firmware/updating/README.md) for updating firmware.
- Copy the "rock_paper_scissors" [folder](..) to the \applications_user\rock_paper_scissors folder in your firmware.
- Build &amp; deploy the firmware. See this [tutorial](/firmware/updating/README.md) for updating firmware.
- NOTE: You can also copy rock_paper_scissors.FAP from latest\.extapps folder and use qFlipper to copy the file to the SD Card/apps/Misc folder.


## HackRF One

- If you do not have two Flipper Zero devices, you can use a HackRF One to record messages &amp; broadcast those messages at a future date. I made a [YouTube](https://www.youtube.com/watch?v=S0sgcDQrVOc) video demo of how to record and broadcast messages.

- To record a message: (replace "flipper-chat.rf" with the file name you want to use, such as "select-rock.rf".)

```
sudo hackrf_transfer -r flipper-chat.rf -f 433920000 -s 8000000
```

- To broadcast a message: (replace "flipper-chat.rf" with the saved file name.)

```
sudo hackrf_transfer -r flipper-chat.rf -f 433920000 -s 8000000 -x 47
```

- What I typically do is:
  - Use the Flipper Zero to send a messasge that I record, then I play back that message at a later time when I want to pretend the other Flipper Zero is sending a message. You can use the chat app in https://lab.flipper.net/cli, like shown in the video to send a specific packet (or you can use your own code to create the packet.)
  - Use the HackRF One to record the message from my Flipper Zero. Then later I use the chat app in https://lab.flipper.net/cli, to see what the message was (or I replay the message to see how my application would respond.)


## Details about the project files

- application.fam

  - specifies the name of our application.
  - specifies the entry point for our application.
  - specifies we use the GUI.
  - specifies our icon is the rock_paper_scissors.png file.
  - specifies our application can be found in the "Game" category.

- rock_paper_scissors.png

  - The icon for our application.

- rock_paper_scissors.c
  - This is the game applcation (tutorial coming soon).


## Example data

- Beacon for game 042:
  Lumyphut: RPS:BA042

- Not Beacon for game 042:
  Lumyphut: RPS:NA042

- Join game 042:
  Lumyphut: RPS:JA042NYourNameHere

- Join-ack for game 042:
  Lumyphut: RPS:AA042NYourNameHere

- Count 1 for game 042:
  Lumyphut: RPS:MA0421

- Count 2 for game 042:
  Lumyphut: RPS:MA0422

- Rock for game 042:
  Lumyphut: RPS:MA042R

- Paper for game 042:
  Lumyphut: RPS:MA042P

- Scissors for game 042:
  Lumyphut: RPS:MA042S
