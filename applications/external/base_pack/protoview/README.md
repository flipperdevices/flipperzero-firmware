ProtoView is a digital signal detection, visualization, editing and reply tool for the Flipper Zero. The Flipper default application, called Subghz, is able to identify certain RF protocols, but when the exact protocol is not implemented (and there are many undocumented and unimplemented ones, such as the ones in use in TPMS systems, car keys and many others), the curious person is left wondering what the device is sending at all. Using ProtoView she or he can visualize the high and low pulses  
This is often enough to make an initial idea about the encoding used
and if the selected modulation is correct. For example, in the signal above
you can see a set of regular pulses and gaps used for synchronization, and then
a sequence of bits encoded in Manchester - https://en.wikipedia.org/wiki/Manchester_code line code. If you study these things for five minutes, you'll find yourself able to decode the bits with naked eyes.

## Decoding capabilities

Other than showing the raw signal, ProtoView is able to decode a few interesting protocols:

* TPMS sensors: Renault, Toyota, Schrader, Citroen, Ford.
* Microchip HSC200/300/301 Keeloq protocol.
* Oregon thermometer protocol 2.
* PT2262, SC5262 based remotes.
* ... more will be implemented soon, hopefully. Send PRs


The app implements a framework that makes adding and experimenting with new
protocols very simple. Check the (protocols) directory to see how the
API works, or read the full documentation at the end of (README) file.
The gist of it is that the decoder receives the signal already converted into
a bitmap, where each bit represents a short pulse duration. Then there are
functions to seek specific sync/preamble sequences inside the bitmap, to decode
from different line codes, to compute checksums and so forth.

## Signals transmission capabilities

Once ProtoView decodes a given message, it is able to *resample* it
in pulses and gaps of the theoretical duration, and send the signal again
via the Flipper TX capabilities. The captured signal can be sent
to different frequencies and modulations than the ones it was captured
from.

For selected protocols, that implement the message creation methods,
ProtoView is also able to edit the message received, modify fields,
and finally re-detect the new produced signal and resend it. Signals
can also be produced from scratch, by setting all the fields to appropriate
values.

## A well-documented app for the Flipper

The secondary goal of ProtoView is to provide a well-documented application for the Flipper (even if ProtoView is a pretty atypical application: it doesn't make use of the standard widgets and other abstractions provided by the framework).
Most apps dealing with the *subghz subsystem* of the Flipper (the abstraction used to work with the CC1101 chip - https://www.ti.com/product/CC1101) tend to be complicated and completely undocumented.
Unfortunately, this is also true for the firmware of the device.
It's a shame, because especially in the case of code that talks with hardware peripherals there are tons of assumptions and hard-gained lessons that can only be captured by comments and are in the code only implicitly - http://antirez.com/news/124

However, the Flipper firmware source code is well written even if it
lacks comments and documentation (and sometimes makes use of abstractions more convoluted than needed), so it is possible to make some ideas of how things work just grepping inside. In order to develop this application, I ended reading most parts of the firmware of the device.

## Detection algorithm

In order to detect and show unknown signals, the application attempts to understand if the samples obtained by the Flipper API (a series of pulses that are high
or low, and with different duration in microseconds) look like belonging to
a legitimate signal, and aren't just noise.

We can't make assumptions about
the encoding and the data rate of the communication, so we use a simple
but relatively effective algorithm. As we check the signal, we try to detect
long parts of it that are composed of pulses roughly classifiable into
a maximum of three different duration classes, plus or minus a given percentage.
Most encodings are somewhat self-clocked, so they tend to have just two or
three classes of pulse lengths.

However, often, pulses of the same theoretical
length have slightly different lengths in the case of high and low level
(RF on or off), so the detector classifies them separately for robustness.

Once the raw signal is detected, the registered protocol decoders are called
against it, in the hope some of the decoders will make sense of the signal.

# Usage

In the main screen, the application shows the longest coherent signal detected so far. The user can switch to other views pressing the LEFT and RIGHT keys. The BACK key will return back to the main screen. Long pressing BACK will quit the application.

## Main raw signal screen

* A long press of the OK button resets the current signal, in order to capture a new one.
* The UP and DOWN buttons change the scale. Default is 100us per pixel, but it will be adapted to the signal just captured.
* A long press of the LEFT and RIGHT keys will pan the signal, to see what was transmitted before/after the current shown range.
* A short press to OK will recenter the signal and set the scale back to the default for the specific pulse duration detected.

Under the detected sequence, you will see a small triangle marking a
specific sample. This mark means that the sequence looked coherent up
to that point, and starting from there it could be just noise. However the
signal decoders will not get just up to this point, but will get more:
sometimes the low level detector can't make sense of a signal that the
protocol-specific decoder can understand fully.

If the protocol is decoded, the bottom-left corner of the screen
will show the name of the protocol, and going in the next screen
with the right arrow will show information about the decoded signal.

In the bottom-right corner the application displays an amount of time
in microseconds. This is the average length of the shortest pulse length
detected among the three classes. Usually the *data rate* of the protocol
is something like (1000000/this-number*2), but it depends on the encoding
and could actually be (1000000/this-number*N) with (N > 2) (here 1000000
is the number of microseconds in one second, and N is the number of clock
cycles needed to represent a bit).

## Info screen

If a signal was detected, the info view will show the details about the signal. If the signal has more data that a single screen can fit, pressing OK will show the next fields. Pressing DOWN will go to a sub-view with an oscilloscope-alike representation of the signal, from there you can:

1. Resend the signal, by pressing OK.
2. Save the signal as (.sub) file, by long pressing OK.

When resending, you can select a different frequency and modulation if you
wish.

## Frequency and modulation screen

In this view you can just set the frequency and modulation you want to use.
There are special modulations for TPMS signals: they need an higher data
rate.

* Many cheap remotes (gate openers, remotes, ...) are on the 433.92Mhz or nearby and use OOK modulation.
* Weather stations are often too in the 433.92Mhz OOK.
* For car keys, try 433.92 OOK650 and 868.35 Mhz in OOK or 2FSK.
* For TPMS try 433.92 in TPMS1 or TPMS2 modulations (FSK and OOK custom modulations optimized for these signals, that have a relatively high data rate).

## Signal creator

In this view, you can do two things:

1. Select one of the protocols supporting message creation, and create a signal from scratch.
2. If there is already a detected signal, you can modify the signal.

This is how it works:

1. Select one of the protocols (the one of the currently detected signal will be already provided as default, if any, and if it supports message creation).
2. Fill the fields. Use LEFT and RIGHT to change the values of integers, or just press OK and enter the new value with the Fliper keyboard widget.
3. When you are done, long press OK to build the message. Then press BACK in order to see it.
4. Go to the INFO view, and then DOWN to the signal sending/saving subview in order to send or save it.

## Direct sampling screen

This final screen shows in real time the high and low level that the Flipper
RF chip, the CC1101, is receiving. This will makes very easy to understand
if a given frequency is targeted by something other than noise. This mode is
fun to watch, resembling an old CRT TV set.

# License

The code is released under the BSD license.

# Disclaimer

This application is only provided as an educational tool. The author is not liable in case the application is used to reverse engineer protocols protected by IP or for any other illegal purpose.

# Credits

A big thank you to the RTL433 author, Benjamin Larsson - https://github.com/merbanan I used the code and tools he developed in many ways:
* To capture TPMS data with rtl433 and save to a file, to later play the IQ files and speedup the development.
* As a sourve of documentation for protocols.
* As an awesome way to visualize and understand protocols, via these great web tools - https://triq.org/
* To have tons of fun with RTLSDR in general, now and in the past.

The application icon was designed by Stefano Liuzzo.
