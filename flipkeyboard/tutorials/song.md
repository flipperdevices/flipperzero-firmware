# Play a song using FlipKeyboard tones

## Introduction

This tutorial will learn to play the Happy Birthday song with FlipKeyboard.

## Step 1: For best results, delete the default configuration.

- Click on the Files tab in qFlipper
- Open `SD Card`
- Open `apps_data`
- Open `flipboard`
- Right click on `keyboard.txt` and Download (if you want a copy that you can copy back later).
- Right click on `keyboard.txt` and choose 'Delete' (and confirm the Delete action.)

## Step 2: Open the FlipKeyboard app

- On your Flipper Zero, open `Apps` folder.
- Choose `GPIO` app.
- Choose `Flipboard Keyboard` app.

## Step 3: Configure the buttons

- Choose `Config` from the main menu of the FlipKeyboard.
- For each Action, set the Music Note (at least for the notes marked with `*`).  You can also set the Press color and Release color.
- Action 1: C5     [1 0 0 0] *
- Action 2: C#5    [0 2 0 0]
- Action 3: D5     [1 2 0 0] *
- Action 4: D#5    [0 0 4 0]
- Action 5: E5     [1 0 4 0] *
- Action 6: F5     [0 2 4 0] *
- Action 7: F#5    [1 2 4 0]
- Action 8: G5     [0 0 0 8] *
- Action 9: G#5    [1 0 0 8]
- Action 10: A5    [0 2 0 8] *
- Action 11: A#5   [1 2 0 8] *
- Action 12: B5    [0 0 4 8]
- Action 13: C6    [1 0 4 8] *

## Step 4: Play the Happy Birthday Song

Choose the `Flipboard Keyboard` option.  Press the buttons to play the song.

- verse 1: 1 1 3 1 6 5  "Hap-py Birth-day to you."
  - [1 0 0 0]
  - [1 0 0 0]
  - [1 2 0 0]
  - [1 0 0 0]
  - [0 2 4 0]
  - [1 0 4 0]

- verse 2: 1 1 3 1 8 6  "Hap-py Birth-day to you."
  - [1 0 0 0]
  - [1 0 0 0]
  - [1 2 0 0]
  - [1 0 0 0]
  - [0 0 0 8]
  - [0 2 4 0]

- verse 3: 1 1 13 10 6 5 3  "Hap-py Birth-day dear Flip-Board,"
  - [1 0 0 0]
  - [1 0 0 0]
  - [1 0 4 8]
  - [0 2 0 8]
  - [0 2 4 0]
  - [1 0 4 0]
  - [1 2 0 0]

- verse 4: 11 11 10 6 8 6  "Hap-py Birth-day to you!"
  - [1 2 0 8]
  - [1 2 0 8]
  - [0 2 0 8]
  - [0 2 4 0]
  - [0 0 0 8]
  - [0 2 4 0]

## Step 5: Create your own song and share it with us!

- [Join Discord Server](https://discord.gg/H89Jzjty6m)

- Share your song in the `#flipboard` channel.

- Here is an example of "Jingle Bells":

  - Start from G. Do NOT program sharps (like G#).
  - 1=**G4** `1000`, 2=**A4** `0200`, 3=**B4** `1200`, 4=**C5** `0040`, 5=**D5** `1040`, 6=**E5** `0240`, 7=**F5** `1240`, 8=**G5** `0008`, 9=**A5** `1001`.

```
  "Da-shing-through-the-snow" (16541) "in a one horse o-pen sleigh" (1116542) 
  "O'er the fields we go" (27653) "Lau-ghing all the way" (88756) 
  "Bells on bob-tail ring" (16541) "ma-king spi-rits bright" (16542) 
  "what fun it is to ride and sing" (22765888) "a slei-ghing song to-night! Oh" (8987548) 
  "Jin-gle bells jin-gle bells" (666666) "jin-gle all the way" (68456) 
  "Oh! What fun it is to ride" (7777766) "in a one-horse o-pen sleigh" (66655658) 
  "jin-gle bells jin-gle bells" (666666) "jin-gle all the way" (68456) 
  "Oh! what fun it is to ride" (7777766) "in a one horse o-pen sleigh" (6688754).
```
