# FAQ

Q: What is that?

A: This app implements nested and static nested attacks (nonces collection part)

Q: Why can't I recover keys on Flipper Zero itself?

A: In fact, you can. There is already an implementation of https://github.com/noproto/FlipperMfkey, and although the author has done a great job with optimization, for a nested attack it would just take a huge amount of time (~~2 hours in worst case for 1 key in regular~~ 30 minutes for 1 key, days in worst case for 1 key in delay + PRNG prediction). Maybe in a great future we will be able to know the exact PRNG value and then we can fit ourselves into these 30 minues. So it's all down to my lack of knowledge of the low-level NFC protocol/Flipper Zero itself.

Q: I don't have keys, how I can read tag?

A: You need to use darkside/reader attack. Reader attack is already implemented in Flipper firmware. Darkside attack isn't available now.

Q: App says "Scan tag and find at least one key to start", but I already scanned tag and found keys. What I need to do?

A: You need to save tag. More -> Save after scanning tag.

Q: How I can recover keys?

A: You need script: https://github.com/AloneLiberty/FlipperNestedRecovery

Q: App name is "Mifare Nested" or "Flipper Nested"?

A: In the early stages of development it was called Mifare Nested. But then I thought it was just better to avoid Mifare in the name, so I replaced it with Flipper. "Flipper Nested" does not really make it clear what this app is for, so it is called "Flipper (Mifare) Nested" in the app list. App can be called by any name: Flipper Nested, Mifare Nested, Flipper (Mifare) Nested, Mifare Nested Attacks for Flipper Zero, etc... But I prefer Flipper Nested.

Q: How to check Flipper Zero/app logs?

A: You need access to CLI. You can use lab.flipper.net (if you are chromium user) or use ./fbt cli. Then type: log debug.

Q: How I can recover keys on phone?

A: Probably you can't. The only way is to forward serial port to Termux and run script (never tried it).

Q: Where are collected nonces/found keys located?

A: App/desktop script write files in /ext/nfc/.nested/ folder. "Check found keys" adds keys directly to your user dictionary.

Q: I recovered keys via script, but still no new keys found when I try to scan tag?

A: You need to run "Check found keys" in app, it will check keys on your tag and add valid to user dictionary. Then you can scan it via NFC app.

Q: What is nonces?

A: By nonces I mean authorization attempts on the tag. Because we can predict PRNG of the tag we can recover keys used in authorization attempts. If you want more info about Nested attach you can read [this paper](https://www.cs.umd.edu/~jkatz/security/downloads/Mifare3.pdf).

Q: How I can ask for help?

A: Create issue **with label "Question"** or use [discussions](https://github.com/AloneLiberty/FlipperNested/discussions). If you can't share private info in issue you can contact me in [Telegram](https://t.me/libertydev). 

Q: What is being done at the moment and what are the plans?

A: Check TODO.md. 

Q: When darkside/hardnested attacks?

A: Hardnested attack is planned, I recovered keys successfully. I can't tell exact time when it will be finished, probably end of April.

Darkside requires accurate PRNG prediction that we can't implement yet (Nested way probably won't work). Maybe I missed something...
