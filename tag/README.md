# Game of tag

**IN DEVELOPMENT**

![Build status](https://github.com/instantiator/flipper-zero-experimental-apps/actions/workflows/build-tag.yml/badge.svg?branch=main)

A game of tag - using the IR and sub-ghz capabilities of the Flipper to coordinate.

## Progress

- [x] Implement IR send/receive
- [x] Implement message queue
- [x] Implement skeleton UI
- [x] Implement basic state transitions (ready/playing/finished)
- [ ] Implement subghz send/receive
- [ ] Integrate subghz with message queue
- [ ] Implement game logic
  - [ ] Firing sends anm IR message with an appropriate id (repeated N times = [duration of send])
  - [ ] On detection of a _relevant_ IR message, damage is recorded
  - [ ] After receiving a message, the same message is ineffective for [duration of send]
  - [ ] After firing and on damage taken, power drops to 0 and must recharge
  - [ ] Power recharges at a steady rate (about 3s until ready?)
  - [ ] On receiving damage, a subghz message with unique id is sent to indicate who hit whom
  - [ ] On receiving this subghz message, if appropriate record a strike score
- [ ] Use new graphics from Kuronons
  - [ ] Hit and Damage popups show for N secs (with vibe and sound) after a hit/damage
  - [ ] Hits and damage stats are updated after strikes
  - [ ] Before playing the player can select their id, and subghz frequency
  - [ ] After playing, the player can see their stats and choose to play again

## Requirements

For build instructions, see the root [README](../README.md).

## Distribution

* When ready, this app will be distributed on [flipc.org](https://flipc.org) (unofficial app catalogue)
