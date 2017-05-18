## xmlTrivialCallbackParser
Trivial XML callback parser. Inspired by [xread](https://github.com/naleksiev/xread.git).

This is a standard C implementation (xread uses GCC non-standard label (&&label) pointers).

It can also be used to "*validate*" XML by passing a NULL callback and verifying the return offset is the same as the source length.
