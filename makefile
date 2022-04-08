clean:
	rm main_threads
	rm main

build: main.c
	gcc -pthread threadpool.c main_threads.c -o main_threads
	gcc -pthread threadpool.c main_threads.c -o main

run: build
	./main_threads
