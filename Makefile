CFLAGS = -I. -Os -g

all: xml.o coders main dom print

clean:
	rm -f xml.o coders main dom print

xml.o: xml.c xml.h
	$(CC) $(CFLAGS) -c xml.c

coders: test/coders.c xml.h xml.o
	$(CC) $(CFLAGS) -o coders test/coders.c xml.o

main: test/main.c xml.h xml.o
	$(CC) $(CFLAGS) -o main test/main.c xml.o

dom: test/dom.c xml.h xml.o
	$(CC) $(CFLAGS) -o dom test/dom.c xml.o

print: test/print.c xml.h xml.o
	$(CC) $(CFLAGS) -o print test/print.c xml.o

check: coders main dom print
	./coders
	./main 0 test/test.xml
	./main 1 test/test.xml
	./dom test/test.xml
	./print < test/test.xml
