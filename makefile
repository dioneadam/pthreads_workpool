clean:
	rm bin/main

build: main.c
	gcc -pthread tpool.c main.c -o bin/main

run: build
	./bin/main
