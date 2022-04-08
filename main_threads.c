#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h> 
#include "threadpool.h"

#define THREADS 4
#define FILE_PATH "inputs/e2.in"

int count_added = 0;
int count_done = 0;

int total = 0;
int numbers_size = 0;

int* read_file();
void job(void* arg);
 
int main(void) { 

  threadpool_t pool;
  
  // chama a função read_file() para ler o arquivo
  // recebe um pointeiro para um array contendo os números
  int *numbers = read_file();
 
  threadpool_create(&pool, THREADS, numbers_size); 
 
  for (int i = 0; i < numbers_size; i++) { 
    threadpool_add_task(pool, job, &numbers[i]); 
    count_added++;
  } 

  printf("Added %d tasks\n", count_added); 
 
  threadpool_destroy(pool, 1); 

  printf("Finish %d tasks\n", count_done);
  printf("\nTotal de divisores: %d\n", total);
 
  return 0; 
}

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
