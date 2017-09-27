## xmlTrivialCallbackParser
Trivial XML callback parser. Inspired by [xread](https://github.com/naleksiev/xread.git).

This is a standard C implementation (xread uses GCC non-standard label (&&label) pointers).

It can also be used to "*validate*" XML by passing a NULL callback and verifying the return offset is the same as the source length.

Also included are some "coders", often used by xml, that I'll break out into a separate repo later:

* URI encode and decode
* Hex encode and decode
* Base64 encode and decode
