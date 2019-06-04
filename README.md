## xmlTrivialCallbackParser
Trivial XML callback parser in standard C.

No dependencies. No dynamic memory allocation. No recursion. No memory copying. Small and Fast.

No XML validation! To simply "*check*" that a XML string can be parsed, use a NULL callback and verify the return offset is the same as the source length.

Also included are some "coders", often used by XML:

* URI encode and decode
* Hex encode and decode
* Base64 encode and decode

Some examples of using this:

* test/main.c: example of using the coders (./main coder) and parsing XML (./main 1 test/test.xml) preserving body whitespace.
* test/print.c: XML on standard input and indented XML on standard output (./print < test/test.xml) ignoring body whitespace.
