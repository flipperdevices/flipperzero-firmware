# Rock Papper Scissors
## Status
This is current a work in progress.  
- Most of the game logic is complete (two flippers should be able to play each other.)
- UI to show images instead of text.

Remaining work:
- Tone when button pressed.
- Show games found & let user pick the game to join.
- Log joined game into SD card.
- Log game results into SD card. 
- Allow viewing past games/scores.
- Config - Allow changing game number.
- Config - Allow changing frequency.
- Config - Allow changing message on join.

## Introduction
This is game of Rock, Paper, Scissors.  In version 1, we use the subghz radio to find other players, communicate moves, and exchange contact information.

## Installation Directions
This project is intended to be overlayed on top of an existing firmware repo.
- Clone, Build &amp; Deploy an existing flipper zero firmware repo.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- Copy the "rock_paper_scissors" [folder](..) to the \applications\plugins\rock_paper_scissors folder in your firmware.
- Build &amp; deploy the firmware.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- NOTE: You can also extract the rock_paper_scissors.FAP from resources.tar file and use qFlipper to copy the file to the SD Card/apps/Misc folder.
### Open question - Should we use "applications" or "applications_user"?

## Running the updated firmware
These directions assume you are starting at the flipper desktop.  If not, please press the back button until you are at the desktop.

- Press the OK button on the flipper to pull up the main menu.
- Choose "Applications" from the menu.
- Choose "Games" from the sub-menu.
- Choose "Rock Paper Scissors"

- Do the same steps on your second Flipper.

- On one of the flippers press Left arrow to join the game.

- Once two players are joined:
  - Press "OK" to send "1".  The other player should also press "OK" to send "1" back (at the same time!)
  - Press "OK" to send "2".  The other player should also press "OK" to send "2" back (at the same time!)
  - Press "UP" to send a "Rock", or "RIGHT" to send a "Paper", or "DOWN" to send a "Scissors".  The other player should send at same time!
  - Rules:
    - Rock beats Scissors.
    - Paper beats Rock.
    - Scissors beat Paper.
    - Two identical items tie.

- Press the BACK button to exit.


## HackRF One
- If you do not have two Flipper Zero devices, you can use a HackRF One to record messages &amp; broadcast those messages at a future date.  I made a [YouTube](https://www.youtube.com/watch?v=S0sgcDQrVOc) video demo of how to record and broadcast messages.

- To record a message: (replace "flipper-chat.rf" with the file name you want to use, such as "select-rock.rf".)
```
sudo hackrf_transfer -r flipper-chat.rf -f 433920000 -s 8000000
```

- To broadcast a message: (replace "flipper-chat.rf" with the saved file name.) 
```
sudo hackrf_transfer -r flipper-chat.rf -f 433920000 -s 8000000 -x 47
```

- What I typically do is:
  - Use the Flipper Zero to send a messasge that I record, then I play back that message at a later time when I want to pretend the other Flipper Zero is sending a message.  You can use the chat app in https://lab.flipper.net/cli, like shown in the video to send a specific packet (or you can use your own code to create the packet.) 
  - Use the HackRF One to record the message from my Fliiper Zero.  Then later I use the chat app in https://lab.flipper.net/cli, to see what the message was (or I replay the message to see how my application would respond.)



## Details about the project files
- application.fam
  - specifies the name of our application.
  - specifies the entry point for our application.
  - specifies we use the GUI.
  - specifies our icon is the rock_paper_scissors.png file.
  - specifies our application can be found in the "Misc" category.

- rock_paper_scissors.png
  - The icon for our application that shows up in the "Misc" folder.

- rock_paper_scissors.c
  - This is the game applcation.