# flipper-logic_analyzer
I was missing a simple way of using Flipper as logic analyzer with e.g. PulseView, so I created a (prototype) plugin.

Just start the Flipper application and you will see smth like that for now:

[Video](http://cdn.discordapp.com/attachments/1074401633615749230/1074405882923864174/pulseview_2023-02-12_20-03-11.mp4 "Video")

Then start PulseView and add a new "Openbench Logic Sniffer (ols)"
and select the second flipper serial port.

When arming, you can now look at the trace in PulseView.

Current state:
 - all 8 channels supported Channel 0 is C0, Channel 1 is C1, ... Channel 7 is A7
 - fixed sampling rate not supported (yet?)
 - if a trigger level is defined, no matter which one, the signals are captured as soon this signal changes
 - maximum capture rate unclear. didn't make any tests. guess in the 100kHz range
 - sample count capped to 16384 for now. didn't check what is possible using malloc()
 - only ONE SHOT currently supported. unknown reason. you have to close and reopen the capture window in PulseView (probably bug in PulseView?)

Discussion thread: https://discord.com/channels/740930220399525928/1074401633615749230
 
