# Programmer Calculator

## Introduction
A Calculator for Programmers!


## Basic Controls

- "**<**" (Backspace) : Press the < button to delete the last digit from the input field. This can be used to correct mistakes or modify your input before conversion.
- "**=**" (Convert): Press the = button to convert the input digits according to the selected mode. Ensure your input is within the mode's threshold for a successful conversion.
- "**CLR**" (Clear): Use the CLR button to clear both the input and output fields, resetting them for a new conversion.

## Selecting Modes

Press "**M**" to cycle through and select the conversion mode you need. 
- Each mode has its specific use and threshold limits:


| Conversion Mode       | Description              | Threshold             |
|-----------------------|--------------------------|-----------------------|
| **dec >> bin**        | Decimal to Binary        | up to \(2^35 - 1\)    |
| **dec >> hex**        | Decimal to Hexadecimal   | up to \(2^64 - 1\)    |
| **dec >> char**       | Decimal to Character     | 0 – 255               |
| **hex >> bin**        | Hexadecimal to Binary    | up to 64-bit inputs   |
| **hex >> dec**        | Hexadecimal to Decimal   | up to \(2^64 - 1\)    |
| **bin >> dec**        | Binary to Decimal        | up to \(2^64 - 1\)    |
| **bin >> hex**        | Binary to Hexadecimal    | up to 64-bit inputs   |


## Confirming Selections
"**OK**": After choosing your desired mode with "**M**", press the OK button to confirm your selection and proceed with the conversion.

## TODO
- Refactor Code
- Memory Management
- Optimize Conversions
- Add more Conversions
- Documentation