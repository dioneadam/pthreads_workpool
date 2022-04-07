#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h> 
#include "tpool.h"

#define THREADS 4

int count_added = 0, count_done = 0;
int total = 0;
 
void job(void* arg) 
{ 
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
 
int main(void) 
{ 
  FILE *pont_arq;
  char content[20];
  
  pont_arq = fopen("inputs/e1.in", "r");

  // read the fisrt line of file, that contains the numbers quantity
  fgets(content, 20, pont_arq);
  int tasks_quantity = atoi(content);

  int numbers[tasks_quantity];

  // continue read the file
  int count = 0;
  while(fgets(content, 20, pont_arq)!= NULL) {
    numbers[count] = atoi(content);
    count ++;
  }
  
  //close file
  fclose(pont_arq);

  tpool_t test_pool; 
 
  tpool_init(&test_pool, THREADS, tasks_quantity); 
 
  for (int i = 0; i < tasks_quantity; i++) { 
    tpool_add_work(test_pool, job, &numbers[i]); 
    count_added++;
  } 

  printf("Added %d tasks\n", count_added); 
 
  tpool_destroy(test_pool, 1); 

  printf("Finish %d tasks\n", count_done);

  printf("\nTotal de divisores: %d\n", total);
 
  return 0; 
}
