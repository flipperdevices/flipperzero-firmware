# Changelog

## 1.5.1

Fix wrong invalid/skipped keys count

## 1.5.0 

Changes:

 - Added new scene with information about why nonces aren't collected (skipped/invalid)

 - Removed some old code, breaking compability with old firmware

 - App renamed from "Flipper (Mifare) Nested" to "Flipper Nested"


## 1.4.6

Fix fallback to hardnested 


## 1.4.5 

Fix .nested folder if running Hard Nested 


## 1.4.4

Fix skipped = false


## 1.4.3

Fix invalid free count


## 1.4.2

Minor code refactor


## 1.4.1

Fix invalid nonces after skipping 


## 1.4.0

Changes:

 - Changed scenes, now they are more informative and beautiful (closes #6)
 
 - Check if sector is alive in Nested attacks (closes #5)
 
 - Now tag PRNG detected at sector, where key is available (fix not working with dead 0 sector, closes #4)
 
 - Detect hard PRNG from start, hardnested doesn't require calibration now
 
 - Settings menu: ability to always run Hard Nested (regardless of PRNG)
 
 - Minor code refactoring, a lot of bug fixes (memory leaks, stability improvements)
 
 - Fallback to Hard Nested now after 25 failed tries (was 10)


## 1.3.0

Check first_byte_sum (no more invalid ones) 


## 1.2.5

Add missing file


## 1.2.4

Missing free() on crypto1


## 1.2.3

Missing free()


## 1.2.2

Migrate to file_stream


## 1.2.1

Fix memory leaks 


## 1.2.0

Hard Nested attack 


## 1.1.1

Improve calibration cycle


## 1.1.0

Changes:

 - Change nested folder name to .nested
 
 - Remove .keys file after search
 
 - Bug fixes


## 1.0.0

Public release 
