# Quantum Fire (QF-6LR)

## Device
These Flipper Zero subghz captures are from the remote control for the Quantum Fire (QF-6LR).  This remote operates at 433.91MHz.  There are 12 buttons (A 1-6 and B 7-12).  I captured all 6 of the A buttons, and button 7 from the B buttons.  

- Model: DBR04 (Quantum Fireworks USA) 
- FCC ID: [Z2F-DBR04](https://fccid.io/Z2F-DBR04)


## Demo
[YouTube video](https://youtu.be/8EuNqTr02eU) of igniting fireworks using the Flipper Zero.  The signals broadcast from the subghz were pre-recorded ("Read RAW", config: 433.92MHz, AM650, RSSI -70) using a remote that was paired to the ignitor.  RAW_a1.sub and RAW_a2.sub were the files used in the video recording.  Princeton_a1 was captured using "Read" instead of "Read RAW".

## Notes
According to fccid.io user manual:
- Frequency is 433MHz
- Coding scheme is "Leaning code (EV1527)"
- Speed is &lt;10Kbps
- Modulation is "OOK (AM)" 

Reading more about EV1527 (non-rolling code, but large key space):
- https://ripplesecurity.com.au/blogs/news/ev1527-and-ask-explained-simple says "Note that in the common basic protocol the full transmission (remote control keypress) consists of a preamble of one appr.. 320 microseconds pulse (“1”) followed by 31 zeros of the same time size (320 X 31 ~ 9900 micros, immediately followed by the 20 bits address and 4 command (button) bits all of the same time size."

The protocol is the Princeton protocol.