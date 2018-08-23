## xmlTrivialCallbackParser
Trivial XML callback parser in standard C. Inspired by [xread](https://github.com/naleksiev/xread.git). (xread uses GCC non-standard label (&&label) pointers.)

No dependencies. No dynamic memory allocation. No memory copying. Small and Fast.

No XML validation. To simply "*check*" that a XML string can be parsed, use a NULL callback and verify the return offset is the same as the source length.

Also included are some "coders", often used by XML, that I'll break out into a separate repo later:

* URI encode and decode
* Hex encode and decode
* Base64 encode and decode

Some examples of using this:

* test/main.c: example of using the coders (./main coder) and parsing nasty XML constructs (./main 1 test/test.xml).
* test/print.c: XML on standard input and indented XML on standard output (./print < test/test.xml).
