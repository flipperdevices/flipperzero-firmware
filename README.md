# flipperzero-yatzee
Yatzee game for flipperzero


Installation: 

Download Yatzee folder and copy to firmware_name/applications_user

connect flipper to PC and run:

./fbt launchapp APPSRC=yatzee


<b>Controls: </b>

- Up to Roll
- Left/Right to move cursor
- OK to Hold a die
- Moving cursor past the dice will move the cursor up to the scorecard. Moving the scores cursor will show you the potential score you would get.


<b>Rules & Scoring:</b>

- Between rolls, move the cursor and use the OK button to select which dice you will hold for the next roll
- 3 rolls per round and then you are forced to select a score. 
- To score, move cursor with Left/Right up to the scorecard, when desired score to count is underlined, press the Down button to confirm.

- 1-6 add up the corresponding dice of that number in your roll.
- 3 of a Kind (3k) = total of dice when 3 of a kind is rolled
- 4 of a Kind (4k) = total of dice when 4 of a kind is rolled
- Full House (Fh) = 25
- Small Straight (Sm) = 30
- Large Straight (Lg) = 40
- Chance (Ch) = total of all dice in roll
- Yatzee (Yz) = 50 for the first yatzee. Successive Yatzees do not show in the score card, but add 100 each to the total score

- Game ends when every scoring value has been selected once.
- If sub score is at least 63, 35 points are added to the total score.
