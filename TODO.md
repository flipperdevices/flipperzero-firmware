# TODO:

1. Better (faster) detection of delay in a nested attack
2. Fix infinite calibration on static encrypted nonce tags

## Thinking about:

1. Files (.nonces and .keys) in Flipper file format (why?)
2. Collect nonces without turning off the tag (no idea how PRNG will react to this and what speed increase will it give?) HALT=0x50
