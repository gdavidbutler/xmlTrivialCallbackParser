CFLAGS = -Wall -Wextra -Iinc -O3 -g

all: main print

clean:
	rm -f xml.o

clobber: clean
	rm -f main print

xml.o: src/xml.c inc/xml.h
	cc $(CFLAGS) -c -o xml.o src/xml.c

main: test/main.c inc/xml.h xml.o
	cc $(CFLAGS) -o main test/main.c xml.o

print: test/print.c inc/xml.h xml.o
	cc $(CFLAGS) -o print test/print.c xml.o

run: main
	./main 0
	./main 0 test/test.xml
	./main 1 test/test.xml
	./print < test/test.xml
