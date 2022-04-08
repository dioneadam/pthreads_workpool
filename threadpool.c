#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <string.h> 
#include <pthread.h> 
#include "threadpool.h"

void *worker(void *); 
int empty(threadpool_t pool);
int full(threadpool_t pool);
int size(threadpool_t pool);
 
void threadpool_create(threadpool_t *threadpool, int pool_size, int tasks_queue_size) { 
  threadpool_t pool = (threadpool_t)malloc(sizeof(struct threadpool)); 
 
  pool->pool_size = pool_size; 
  pool->tasks_queue_size = tasks_queue_size + 1; 
  pool->threads = NULL; 
  pool->head = 0; 
  pool->tail = 0; 
  pool->queue_closed = 0; 
  pool->shutdown = 0; 
 
  pthread_mutex_init(&pool->queue_lock, NULL);
  pthread_cond_init(&pool->can_add_task, NULL);
  pthread_cond_init(&pool->task_notify, NULL);
  pthread_cond_init(&pool->queue_empty, NULL);
 
  pool->tasks = malloc(sizeof(struct threadpool_task) * pool->tasks_queue_size);
 
  pool->threads = malloc(sizeof(pthread_t) * pool_size);
 
  for (int i = 0; i < pool_size; i++) { 
    if (pthread_create(&pool->threads[i], NULL, worker, (void *)pool) != 0) { 
      perror("pthread_create"); 
      exit(0); 
    } 
  }
 
  *threadpool = pool; 
}
 
int threadpool_add_task(threadpool_t tpool, void(*routine)(void *), void *arg) { 
  threadpool_task_t *temp; 
 
  pthread_mutex_lock(&tpool->queue_lock); 
 
  while (full(tpool) && !tpool->shutdown && !tpool->queue_closed) { 
    pthread_cond_wait(&tpool->can_add_task, &tpool->queue_lock); 
  } 
 
  if (tpool->shutdown || tpool->queue_closed) { 
    pthread_mutex_unlock(&tpool->queue_lock); 
    return -1; 
  } 
 
  int is_empty = empty(tpool); 
 
  temp = tpool->tasks + tpool->tail; 
  temp->routine = routine; 
  temp->arg = arg; 
  tpool->tail = (tpool->tail + 1) % tpool->tasks_queue_size; 
 
  if (is_empty) { 
    // signal has task
    pthread_cond_broadcast(&tpool->task_notify); 
  } 
 
  pthread_mutex_unlock(&tpool->queue_lock);   
 
  return 0; 
}
 
int threadpool_destroy(threadpool_t tpool, int finish) { 
  pthread_mutex_lock(&tpool->queue_lock); 
 
  tpool->queue_closed = 1; 
 
  if (finish == 1) { 
    // wait all work be done 
    while (!empty(tpool)) { 
      pthread_cond_wait(&tpool->queue_empty, &tpool->queue_lock); 
    } 
  } 
  tpool->shutdown = 1; 
 
  pthread_mutex_unlock(&tpool->queue_lock); 
 
  pthread_cond_broadcast(&tpool->task_notify); 
 
  // wait worker thread exit 
  for (int i = 0; i < tpool->pool_size; i++) { 
    pthread_join(tpool->threads[i], NULL); 
  } 
 
  // free thread pool 
  free(tpool->threads); 
  free(tpool->tasks); 
  free(tpool); 
}

void *worker(void *arg) { 
  threadpool_t pool = (threadpool_t)(arg); 
  threadpool_task_t *task; 
 
  for (;;) { 
    pthread_mutex_lock(&pool->queue_lock); 
 
    while (empty(pool) && !pool->shutdown) {  
      pthread_cond_wait(&pool->task_notify, &pool->queue_lock); 
    } 
 
    if (pool->shutdown == 1) { 
      pthread_mutex_unlock(&pool->queue_lock); 
      pthread_exit(NULL); 
    } 
 
    int is_full = full(pool); 
    task = pool->tasks + pool->head; 
    pool->head = (pool->head + 1) % pool->tasks_queue_size; 
 
    if (is_full) { 
      pthread_cond_broadcast(&pool->can_add_task); 
    } 
 
    if (empty(pool)) { 
      pthread_cond_signal(&pool->queue_empty); 
    } 
 
    pthread_mutex_unlock(&pool->queue_lock);   
 
    (*(task->routine))(task->arg); 
  } 
}

int empty(threadpool_t pool) { 
  return pool->head == pool->tail; 
} 
 
int full(threadpool_t pool) { 
  return ((pool->tail + 1) % pool->tasks_queue_size == pool->head); 
} 
 
int size(threadpool_t pool) { 
  return (pool->tail + pool->tasks_queue_size - pool->head) % pool->tasks_queue_size; 
}
