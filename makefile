clean:
	rm bin/main_threads

build: main.c
	gcc -pthread threadpool.c main_threads.c -o bin/main_threads

run: build
	./bin/main_threads
