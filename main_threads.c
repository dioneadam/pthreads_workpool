//Nomes: Dione Adam, Julia Boesing
//Computação de Alto Desempenho 2022/01

#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "threadpool.h"

//Quantidade de Threads a serem criadas/executadas
#define THREADS 4

//Arquivo com a lista de números
#define FILE_PATH "inputs/e1.in"

typedef struct timespec Time;

//Contadores auxiliares para verificação de criação de tasks
int count_added = 0;
int count_done = 0;

//Total de divisores encontrados
int total = 0;

//Tamanho da fila de tarefas
int numbers_size = 0;

int* read_file();
void job(void* arg);
 
int main(void) { 
  double time;
  Time start_time, end_time;

  // Tempo inicial
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, (struct timespec *) &start_time);

  threadpool_t pool;
  
  // chama a função read_file() para ler o arquivo
  // recebe um pointeiro para um array contendo os números
  int *numbers = read_file();
  
  // Cria as threads
  threadpool_create(&pool, THREADS, numbers_size); 
  
  for (int i = 0; i < numbers_size; i++) { 
    //Aloca uma tarefa na fila
    threadpool_add_task(pool, job, &numbers[i]); 
    count_added++;
  } 

  printf("Added %d tasks\n", count_added); 
 
  // Termina a execução das threads
  threadpool_destroy(pool, 1);

  // Tempo final
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, (struct timespec *) &end_time); 

  printf("Finish %d tasks\n", count_done);
  printf("\nTotal de divisores: %d\n", total);

  // Converte o tempo de nanossegundos em segundos
  // Calcula o tempo de execução
  time = (double) (end_time.tv_sec - start_time.tv_sec ) +
         (double) (end_time.tv_nsec - start_time.tv_nsec) * 1e-9;
  
  printf("\nTempo de execução: %fs\n", time);
 
  return 0; 
}

// Calcula o total de divisores para um número
// arg é o parametro que recebe o número a ser descoberto os divisores
void job(void* arg) { 
  int number = *(int*)arg;

  int result = 0;
  for(int i=1; i<=number; i++) {
    if (number % i == 0) {
      result++;
    }
  }

  printf("Thread: %p: Total de divisores para %d = %d\n", (void *)pthread_self(), number, result);
 
  total += result;
  count_done++;
}

int* read_file() {
  FILE *file;
  static char line[20];
  
  // abre o arquivo
  file = fopen(FILE_PATH, "r");

  // lê a primeira linha do arquivo, contendo a quantidade de números
  // adiciona esse numero para a variavel number_size
  fgets(line, 20, file);
  numbers_size = atoi(line);

  // aloca memoria para o array dos numeros
  int *numbers = (int *)malloc(sizeof(int) * numbers_size);

  // continua a leitura do arquivo
  // e adiciona cada número no array
  int count = 0;
  while(fgets(line, 20, file)!= NULL) {
    numbers[count] = atoi(line);
    count ++;
  }
  
  // fecha o arquivo
  fclose(file);

  return numbers;
}
