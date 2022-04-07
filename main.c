#include <stdio.h> 
#include <pthread.h> 
#include "tpool.h"

#define THREADS 2
#define TASKS  10

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
  int numbers[]={1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; 

  tpool_t test_pool; 
 
  tpool_init(&test_pool, THREADS, TASKS); 
 
  for (int i = 0; i < TASKS; i++) { 
    tpool_add_work(test_pool, job, &numbers[i]); 
    count_added++;
  } 

  printf("Added %d tasks\n", count_added); 
 
  tpool_destroy(test_pool, 1); 

  printf("Did %d tasks\n", count_done);

  printf("\n\nTotal de divisores: %d\n", total);
 
  return 0; 
}
