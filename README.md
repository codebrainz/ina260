Arduino INA260 Library
======================

Introduction
------------

This is a small library which exposes the functionality of
[Texas Instruments' INA260][0] current-sense amplifier I2C device. It
is necessary to read the [INA260 datasheet][1] first to understand how
the device works. There should be a 1:1 mapping between the datasheet
and the types and functions exposed in this library, wherever possible.

The library also includes several helper and conversion functions which
users of this device may find useful.

Installation
------------

Check-out [the Git repository][2] or extract [a .zip of it][3] to a
directory named `INA260` in your Arduino Sketch directory's `libraries`
sub-directory. Refer to the [Installing Additional Arduino Libraries Guide][4]
for more information on installing Arduino libraries.

Documentation
-------------

The best sources of information are the [TI INA260 datasheet][1] and
the [INA260.h header file][5]. Since this is a thin wrapper over the
INA260 I2C interface, you should be able to work out how to use this
API based on the information provided in the datasheet. See also
[the examples directory][6] for working examples on using the library.

Dependencies
------------

This library uses only the standard [Arduino][7]/[Wire][8] libraries
and modern [C++ standard library][9]/compiler. It makes use of some
[GCC-specific extensions][10] and assumes the target architecture is
little endian. For most uses this doesn't matter, unless you're porting
the library to a new architecture or compiler. The examples assume you
have the Arduino connected by USB/serial and can use the Arduino IDE's
Serial Monitor/Plotter functionality.

[0]: http://www.ti.com/product/INA260
[1]: http://www.ti.com/lit/ds/symlink/ina260.pdf
[2]: https://github.com/codebrainz/ina260.git
[3]: https://github.com/codebrainz/ina260/archive/master.zip
[4]: https://www.arduino.cc/en/Guide/Libraries
[5]: https://github.com/codebrainz/ina260/blob/master/src/INA260.h
[6]: https://github.com/codebrainz/ina260/tree/master/examples
[7]: https://www.arduino.cc/en/Reference/Libraries
[8]: https://www.arduino.cc/en/Reference/Wire
[9]: https://en.wikipedia.org/wiki/C%2B%2B_Standard_Library
[10]: https://gcc.gnu.org/onlinedocs/gcc/C-Extensions.html
