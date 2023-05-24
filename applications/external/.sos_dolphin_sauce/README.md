# dolphin SOS
Sends an SOS CW (morse code) message on 434.0MHz. Works against flipperzero api 0.70.1.

By default, the message transmitted is SMS (••• – – •••) which you might know as the nokia SMS tune. 

The message can be toggled between "SMS" and "SOS" by pressing: left, right, left, right, left, right, enter. 

# technical details

* Currently sending on 434.0 MHz and nothing else. I doubt anyone will even tune to ISM for CW, or consider an ISM-band CW signal anything serious, so I wouldn't rely on flipper for emergencies.
* WPM timing is 166ms per dit. The PARIS system places it at 6-7 WPM. This was chosen so that a trained receiver can assume that the transmitter is not proficient in keying morse code and that he actually uses ISM bands as a real emergency. A good fist (well spaced, fast morse signal) on ISM might seem like a joke than a real emergency
* SOS is a prosign and can be sent without spaces between letters.

# todo

* change freqs and retune
* continuous channel marker for RDF
* maybe CW out some other message, your ID, emergency phone number, callsign, etc
* alter WPM
* adding a bit of randomness (bad fist) might help the receiver assume this isn't another ISM device, but there's an actual human behind it in a real emergency

