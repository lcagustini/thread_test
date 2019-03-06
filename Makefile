all:
	mkdir -p bin
	gcc -o bin/main src/main.c -lSDL2 -lm -lpthread -O2

run: all
	bin/main
