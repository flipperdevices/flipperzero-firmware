# Capturing NFC emulation traces

When debugging emulation problems, a full emulation trace from a Proxmark3 helps us a lot. To capture a trace, you need to have a Proxmark3, a Flipper Zero, a card you're trying to emulate, and a reader you're trying to emulate the card to.

We'll need four things:

1. A sniff of the communication between a reader and a genuine card.
2. A sniff of the communication between a Flipper on a working firmware version and a reader.
3. A sniff of the communication between a Flipper on a current firmware version (the one where things broke).
4. A full card dump (can be acquired with `hf mf autopwn` on the proxmark).

Flipper's `debug` toggle in System settings should be **disabled**, log level should be either `None` or `Default`.

To capture a trace, start recording on your proxmark with the `hf 14a sniff` command, then start the emulation on your Flipper Zero. After that, place the proxmark to the back of the Flipper (as if it were a card you're trying to read), and then move the whole thing to the reader. Once the reader has finished communicating with the Flipper, stop the recording on the proxmark by pressing the button on it. The trace will be saved to the proxmark's flash memory. Don't forget to press the button on your proxmark to stop the recording, otherwise the trace will be lost. After the recording is finished, you can save it to your computer with the `trace save -f <filename>` command.

To capture a trace of a genuine card, you can use the same method, but instead of the Flipper, you'll need to use the card you're trying to emulate.

> **Note:** If you're using a Proxmark3 Easy, you'll probably need to remove the middle plate of your device for the trace to be captured correctly. It is a non-integral part of the proxmark, and is there only to keep the cards at an optimal distance from the antenna while reading. It is not needed for sniffing, and will interfere with the trace capture. After capturing the trace, you can put the plate back on, or leave it off if you don't need it. If you're using a Proxmark3 RDV4, you don't need to remove anything.

For convenience, please name your trace files along the lines of `card`, `working_fw`, `latest_fw`, and `card_dump` (in bin, json, and eml formats), respectively. The file extensions for the traces will be added automatically by your proxmark client.

Once you have all the traces, please create an issue on GitHub and attach them to it. If you have any questions, feel free to ask them in the issue as well.
