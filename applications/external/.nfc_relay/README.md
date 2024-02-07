# nfc_relay

NFC Relay app for flipper zero, relay NFCA APDU and response between two flipper.

## TODO or LIMITATIONS

* TODO
  * configurable UART pin and baudrate (Right now is hardcored using 16/17 and 38400)
    * baudrate bigger than 38400 will case data lost
  * other method for flipper zero to communicate.
* limitations
  * st25r3916 RFAL002 will not emulate 10 bytes NFCID easily
  * `furi_hal_nfc_emulate_nfca` currently are using hardcoded ATS(Answer-to-Select)

## test viedo

[![flipper zero NFC relay test (credit card)](http://img.youtube.com/vi/3GQiGDuhGzE/0.jpg)](https://www.youtube.com/watch?v=3GQiGDuhGzE)

Setup in this video are:

* flipper zero with case + wifi dev board(flashed with esp-at, using `IO17/IO18` as TX/RX, config with wifi network and pass through internet `AT+SAVETRANSLINK=1,"<SERVER_DOMAIN>",7771,"UDP",1005`)
* flipper zero naked + m5stickc(also flashed with esp-at, using pin 25 and 26, config with wifi network and pass through internet `AT+SAVETRANSLINK=1,"<SERVER_DOMAIN>",7770,"UDP",1005`)
* Server on some cloud running socat `socat -v -d -d udp-l:7770 udp-l:7771`
