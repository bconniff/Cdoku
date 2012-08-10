CC=gcc

all: bin/cdoku

bin/cdoku: src/main.c src/stack.c src/solver.c src/stack.c
	mkdir -p bin
	cd src && "${CC}" *.c -o ../bin/cdoku

clean:
	rm -rf bin
