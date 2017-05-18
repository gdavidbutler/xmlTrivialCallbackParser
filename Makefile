CFLAGS = -Wall -Wextra -Iinc -O3 -g

all: main

clean:
	rm -f xml.o

clobber: clean
	rm -f main

xml.o: src/xml.c inc/xml.h
	cc $(CFLAGS) -c -o xml.o src/xml.c

main: test/main.c inc/xml.h xml.o
	cc $(CFLAGS) -o main test/main.c xml.o

run: main
	./main 1 test/test.xml
	./main 0 test/test.xml
