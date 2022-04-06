## xmlTrivialCallbackParser
Trivial XML callback parser in standard C.

No dependencies. No dynamic memory allocation. No recursion. No memory copying. Small and Fast.

This is **not** a validating parser!
To *check* that a XML string can be parsed, use a NULL callback and verify the return offset is the same as the source length.

A C structure tree of a parsed XML document is provided.
For a more complete DOM representation, see my [xmlTrivialDom](https://github.com/gdavidbutler/xmlTrivialDom).

Also included are some "coders", often used by XML:

* URI encode and decode
* Hex encode and decode
* Base64 encode and decode

Some examples of using this:

* test/coders.c: example of using the coders.
* test/main.c: example of checking (./main 0 test/test.xml) and parsing XML (./main 1 test/test.xml) preserving whitespace only bodies.
* test/dom.c: example of using the C structure tree (./dom test/test.xml).
* test/print.c: XML on standard input and indented XML on standard output (./print < test/test.xml) ignoring whitespace only bodies.
