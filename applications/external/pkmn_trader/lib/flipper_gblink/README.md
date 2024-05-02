# Flipper Game Boy Game Link Cable API
Simple API that can be included in projects to provide a flexible and easy way to handle data exchange over a Game Link Cable.

Current Version: 0.6

Available from: https://github.com/kbembedded/flipper-gblink


## Current feature status and future roadmap:
- [x] Ability to use EXT clock source. i.e. connected device drives the clock line  
- [x] Callback on byte transfer completion  
- [x] Flexibility in IO pin selection at alloc time  
- [x] Ability to enable and disable interrupt on input clock  
- [x] Ability to set timeout in microseconds between clock edges. If exceeded, it is assumed the next clock is the first bit of a byte  
- [x] Set a NO\_DATA\_BYTE pattern. i.e. after a byte transfer is complete, a default byte is prepared to be sent out if no new data is provided before the transfer starts  
- [x] Supports communication to GBC  
- [x] Supports communication to GBA using GBC games  
- [x] Supports older MALVEKE pinouts that would previously cause the Okay button to stop functioning after a trade  
- [ ] Supports communication to GB (untested, but should work)  
- [ ] Supports communication to GBA using GBA games  
- [ ] Function as INT clock source. i.e. Flipper Zero drives the clock line  
- [ ] Drive clock at varying speeds as GBC supports  
- [ ] Proper documentation  

## Use example
See https://github.com/EstebanFuentealba/Flipper-Zero-Game-Boy-Pokemon-Trading

To include this in a Flipper Zero application, add this repo as a submodule in the `lib/` directory of the application source. Then add the following to `application.fam`:
```
App(
...
    fap_private_libs=[
        Lib(
            name="flipper-gblink",
        ),
    ],
...
)
```
