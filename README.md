# minmea, a lightweight GPS NMEA 0183 parser library

[![Build Status](https://travis-ci.org/kosma/minmea.svg?branch=master)](https://travis-ci.org/kosma/minmea)

Minmea is a minimalistic GPS parser library written in pure C intended for
resource-constrained platforms, especially microcontrollers and other embedded
systems.

## Features

* Written in ISO C99.
* No dynamic memory allocation.
* No floating point usage in the core library.
* Supports both fixed and floating point values.
* One source file and one header - can't get any simpler.
* Tested under Linux, OS X, Windows and embedded ARM GCC.
* Easily extendable to support new sentences.
* Complete with a test suite and static analysis.

## Supported sentences

* ``RMC`` (Recommended Minimum: position, velocity, time)
* ``GGA`` (Fix Data)
* ``GSA`` (DOP and active satellites)
* ``GLL`` (Geographic Position: Latitude/Longitude)
* ``GST`` (Pseudorange Noise Statistics)
* ``GSV`` (Satellites in view)
* ``VTG`` (Track made good and Ground speed)
* ``ZDA`` (Time & Date - UTC, day, month, year and local time zone)

Adding support for more sentences is trivial; see ``minmea.c`` source. Good documentation
on NMEA is at http://www.catb.org/gpsd/NMEA.html

## Compatibility

Minmea runs out-of-the-box under most Unix-compatible systems. Support for non-Unix systems
(including native Windows builds under MSVC) is provided via compatibility headers:

1. Define `MINMEA_INCLUDE_COMPAT` in the build environment.
2. Add appropriate compatibility header from under `compat/` directory as `minmea_compat.h`.

## Fractional number format

Internally, minmea stores fractional numbers as pairs of two integers: ``{value, scale}``.
For example, a value of ``"-123.456"`` would be parsed as ``{-123456, 1000}``. As this
format is quite unwieldy, minmea provides the following convenience functions for converting
to either fixed-point or floating-point format:

* ``minmea_rescale({-123456, 1000}, 10) => -1235``
* ``minmea_float({-123456, 1000}) => -123.456``

The compound type ``struct minmea_float`` uses ``int_least32_t`` internally. Therefore,
the coordinate precision is guaranteed to be at least ``[+-]DDDMM.MMMMM`` (five decimal digits)
or ±2cm LSB at the equator. Note that GPS modules commonly only provide four deciminal digits
(``[+-]DDDMM.MMMM``), which equates to ±20cm (0.0001 minute is 0.0001/60 degrees and one degree
is about 111km) at the equator.

## Coordinate format

NMEA uses the clunky ``DDMM.MMMM`` format which, honestly, is not good in the internet era.
Internally, minmea stores it as a fractional number (see above); for practical uses,
the value should be probably converted to the DD.DDDDD floating point format using the
following function:

* ``minmea_tocoord({-375165, 100}) => -37.860832``

The library doesn't perform this conversion automatically for the following reasons:

* The conversion is not reversible.
* It requires floating point support.
* The user might want to perform this conversion later on or retain the original values.

## Example

```c
char line[MINMEA_MAX_LENGTH];
while (fgets(line, sizeof(line), stdin) != NULL) {
    switch (minmea_sentence_id(line, false)) {
        case MINMEA_SENTENCE_RMC: {
            struct minmea_sentence_rmc frame;
            if (minmea_parse_rmc(&frame, line)) {
                printf("$RMC: raw coordinates and speed: (%d/%d,%d/%d) %d/%d\n",
                        frame.latitude.value, frame.latitude.scale,
                        frame.longitude.value, frame.longitude.scale,
                        frame.speed.value, frame.speed.scale);
                printf("$RMC fixed-point coordinates and speed scaled to three decimal places: (%d,%d) %d\n",
                        minmea_rescale(&frame.latitude, 1000),
                        minmea_rescale(&frame.longitude, 1000),
                        minmea_rescale(&frame.speed, 1000));
                printf("$RMC floating point degree coordinates and speed: (%f,%f) %f\n",
                        minmea_tocoord(&frame.latitude),
                        minmea_tocoord(&frame.longitude),
                        minmea_tofloat(&frame.speed));
            }
        } break;

        case MINMEA_SENTENCE_GGA: {
            struct minmea_sentence_gga frame;
            if (minmea_parse_gga(&frame, line)) {
                printf("$GGA: fix quality: %d\n", frame.fix_quality);
            }
        } break;

        case MINMEA_SENTENCE_GSV: {
            struct minmea_sentence_gsv frame;
            if (minmea_parse_gsv(&frame, line)) {
                printf("$GSV: message %d of %d\n", frame.msg_nr, frame.total_msgs);
                printf("$GSV: sattelites in view: %d\n", frame.total_sats);
                for (int i = 0; i < 4; i++)
                    printf("$GSV: sat nr %d, elevation: %d, azimuth: %d, snr: %d dbm\n",
                        frame.sats[i].nr,
                        frame.sats[i].elevation,
                        frame.sats[i].azimuth,
                        frame.sats[i].snr);
            }
        } break;
    }
}
```

## Integration with your project

Simply add ``minmea.[ch]`` to your project, ``#include "minmea.h"`` and you're
good to go.

## Running unit tests

Building and running the tests requires the following:

* Check Framework (http://check.sourceforge.net/).
* Clang Static Analyzer (http://clang-analyzer.llvm.org/).

If you have both in your ``$PATH``, running the tests should be as simple as
typing ``make``.

## Limitations

* Only a handful of frames is supported right now.
* There's no support for omitting parts of the library from building. As
  a workaround, use the ``-ffunction-sections -Wl,--gc-sections`` linker flags
  (or equivalent) to remove the unused functions (parsers) from the final image.
* Some systems lack ``timegm``. On these systems, the recommended course of
  action is to build with ``-Dtimegm=mktime`` which will work correctly as long
  the system runs in the default ``UTC`` timezone.

## Bugs

There are plenty. Report them on GitHub, or - even better - open a pull request.
Please write unit tests for any new functions you add - it's fun!

## Licensing

Minmea is open source software; see ``COPYING`` for amusement. Email me if the
license bothers you and I'll happily re-license under anything else under the sun.

## Author

Minmea was written by Kosma Moczek &lt;kosma@kosma.pl&gt; and Patryk Szymczak
&lt;patryk.szymczak@gmail.com&gt; at Cloud Your Car, with bugs fixed by countless
good people.
