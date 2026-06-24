# SubGHz — Test Cases

## 1. Sub 1GHz Read Raw

Steps:
1. Sub-GHz â Read RAW â Config
2. Set frequencies 315, 433, 868 one by one
3. Record signals from different remote controls
4. Record the remote control signal from an encrypted protocol (for example, Came Atomo, gray remote control
   with black buttons)
5. Transmit signals to receivers from the stand one by one

   **Expected result**
5.1. Signals were received by receivers on the stand

## 2. Sub-GHz Frequency Analyzer

Steps:
1. Sub-GHz â Frequency Analyzer
  2. Bring remote controls with different transmission frequencies to the flipper by pressing the button on them
  2.1. the analyzer correctly displays the frequency at which the remote control operates

## 3. Sub-GHz External Module

Description:

Testing on external module CC1101
The module is designed for frequency 433.92, therefore tests must be carried out on it.
The module cannot be turned off during operation, this may cause the flipper to freeze
Steps:
1. Install external module
   1.1. The module has changed to external
   2. Go to Read
   2.1. The icon in the upper right corner has changed to an external module
   3. Go to Read RAW and receive a signal from the 433 remote control
   3.1. The signal became stronger than on the built-in antenna
   4. Switch the module to internal, leaving the module in the flipper and go to Read
   4.1. Reading occurs from the internal module

## 4. Sub-GHz operations with a saved file

Steps:
1. Receive a Sub-GHz signal and save it
   **Expected result**
     1. File saved

2. Receive a RAW Sub-GHz signal and save it
   **Expected result**
     1. File saved

3. Add previously saved files to favorites
4. Launch files from the "favorites" submenu
   **Expected result**
     1. Keys are sent without artifacts

5. Go to Sub-GHz -> Saved, and rename previously created files, and go to favorites
   **Expected result**
     1. The files were renamed and remained in favorites with a new name

6. Delete previously renamed files
   **Expected result**
     1. When deleting, a dialog box appeared, after deleting, the âdeletedâ picture appeared and we were thrown
        into the âsavedâ menu

## 5. Sub-GHz reception

Description:

Checking the reception of radio signals from different remote controls
315,000 MHz  
433.920 MHz
868,350 MHz
Steps:
1. Launch the Sub-1 GHz p â Read application, tune in one by one to frequencies 315, 433, 868MHz
  2. Transmit a signal from the remote control with a frequency of 315,000 MHz
  2.1. A signal with a frequency of 315,000 MHz is received
  3. Transmit a signal from the remote control with a frequency of 433.920 MHz
  3.1. A signal with a frequency of 433.920 MHz is received
  4. Transmit a signal from the remote control with a frequency of 868.350 MHz
  4.1. A signal with a frequency of 868.350 MHz is received
  5. Go to config and enable hopping. After this, repeat reading all signals
  5.1. All signals from each frequency were counted

## 6. Sub-GHz transmission

Description:

Checking signal transmission from the flipper. To send, you can use the keys saved upon receipt.
To check, you can use either a second flipper or a stand 

315,000 MHz
433.920 MHz
868,350 MHz
Steps:
1. If the keys are already saved, then go to Saved and select the saved keys
   or
   Take signals from the keys one by one and press âsendâ
   1.1. The relays on the stand are activated

## 7. Sub-GHz Add Manually

Steps:
1. Sub-GHz â Add manually
   2. Create signals for all available protocols in this section
   3. Transmit signals to receivers from the stand
   3.1. Signals are being received
