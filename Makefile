CFLAGS = -I. -Os -g

all: xml.o main print

clean:
	rm -f xml.o main print

xml.o: xml.c xml.h
	cc $(CFLAGS) -c xml.c

main: test/main.c xml.h xml.o
	cc $(CFLAGS) -o main test/main.c xml.o

print: test/print.c xml.h xml.o
	cc $(CFLAGS) -o print test/print.c xml.o

check: main print
	./main 0
	./main 0 test/test.xml
	./main 1 test/test.xml
	./print < test/test.xml
