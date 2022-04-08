# Pthreads_workpool

#### Nomes: Dione Adam, Julia Boesing
#### Computação de Alto Desempenho 2022/01

---

### Build and run


Execute
```
make build
./main_threads
./main
```

ou execute os seuintes comandos:

#### Sem threads:
```
gcc -pthread threadpool.c main.c -o main
./main
```

#### Com threads:
```
gcc -pthread threadpool.c main_threads.c -o main_threads
./main_threads
```
