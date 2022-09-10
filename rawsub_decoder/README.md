# rawsub_decoder

Decode Flipper zero's raw .sub file in case of OOK, supposing a Manchester encoding.

Odds are the script won't exactly fit your needs, but feel free to adapt it for your protocol.

Bits are decoded depending on the edges of duration between 700 and 900 :
- 0 bit = rising edge (350\~450 no signal then 350\~450 signal)
- 1 bit = falling edge (350\~450 signal then 350\~450 no signal)

## Usage
```
python3 decode_ook_manchester.py -h
```

