CC="gcc -O3 -ansi"

all: bin/cdoku

bin/cdoku: src/main.c src/matrix.c src/reader.c src/solver.c src/stack.c src/xmalloc.c
	mkdir -p bin
	cd src && "${CC}" *.c -o ../bin/cdoku

clean:
	rm -rf bin
