//Nomes: Dione Adam, Julia Boesing
//Computação de Alto Desempenho 2022/01

#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <string.h> 
#include <pthread.h> 
#include "threadpool.h"

void *worker(void *); 
int empty(threadpool_t pool);
int size(threadpool_t pool);

// Faz a criação das threads no threadpool 
void threadpool_create(threadpool_t *threadpool, int pool_size, int tasks_queue_size) { 

  //Aloca memoria para  o threadpool
  threadpool_t pool = (threadpool_t)malloc(sizeof(struct threadpool)); 
 
  //Define valores iniciais
  pool->pool_size = pool_size; 
  pool->tasks_queue_size = tasks_queue_size + 1; 
  pool->threads = NULL; 
  pool->head = 0; 
  pool->tail = 0; 
  pool->queue_closed = 0; 
  pool->shutdown = 0; 
 
  //Inicializa mutex e cond
  pthread_mutex_init(&pool->queue_lock, NULL);
  pthread_cond_init(&pool->task_notify, NULL);
  pthread_cond_init(&pool->queue_empty, NULL);
 
  //Aloca memria para as filas
  pool->tasks = malloc(sizeof(struct threadpool_task) * pool->tasks_queue_size);
  pool->threads = malloc(sizeof(pthread_t) * pool_size);
 
  for (int i = 0; i < pool_size; i++) {
    //Faz a criação da thread
    //Adiciona a rotina de verifição da existencia de tasks na fila 
    if (pthread_create(&pool->threads[i], NULL, worker, (void *)pool) != 0) { 
      perror("pthread_create"); 
      exit(0); 
    } 
  }
 
  // Retorna a referência da threadpool para a main
  *threadpool = pool; 
}

// Adiciona tarefas na fila
int threadpool_add_task(threadpool_t threadpool, void(*routine)(void *), void *arg) { 
  threadpool_task_t *task; 
 
  pthread_mutex_lock(&threadpool->queue_lock); 
 
  //Finaliza a execução caso
  //tenha o sinal para desligar ou a fila esteja fechada
  if (threadpool->shutdown || threadpool->queue_closed) { 
    pthread_mutex_unlock(&threadpool->queue_lock); 
    return -1; 
  } 
 
  int is_empty = empty(threadpool); 
 
  //Instancia task como sendo ultima task na fila
  task = threadpool->tasks + threadpool->tail; 

  //atribui a rotina e o número para task
  task->routine = routine; 
  task->arg = arg; 

  //Avança o tail
  threadpool->tail = (threadpool->tail + 1) % threadpool->tasks_queue_size; 
 
  // caso a fila de tasks esteja vazia, sinaliza que há novas tasks
  if (is_empty) {
    pthread_cond_broadcast(&threadpool->task_notify); 
  } 
 
  pthread_mutex_unlock(&threadpool->queue_lock);   
 
  return 0; 
}
 
//Finaliza a execução 
int threadpool_destroy(threadpool_t threadpool, int finish) { 
  pthread_mutex_lock(&threadpool->queue_lock); 
 
  threadpool->queue_closed = 1; 
 
  if (finish == 1) { 
    // Espera tudo ser processado
    while (!empty(threadpool)) { 
      pthread_cond_wait(&threadpool->queue_empty, &threadpool->queue_lock); 
    } 
  } 
  threadpool->shutdown = 1; 
 
  pthread_mutex_unlock(&threadpool->queue_lock); 
 
  pthread_cond_broadcast(&threadpool->task_notify); 
 
  // Espera todas as tasks finalizarem 
  for (int i = 0; i < threadpool->pool_size; i++) { 
    pthread_join(threadpool->threads[i], NULL); 
  } 
 
  // libera memoria
  free(threadpool->threads); 
  free(threadpool->tasks); 
  free(threadpool); 
}

//Rotina de cada thread
void *worker(void *arg) { 
  threadpool_t pool = (threadpool_t)(arg); 
  threadpool_task_t *task; 
 
  for (;;) {
    pthread_mutex_lock(&pool->queue_lock); 
 
    //verifica se existe alguma task para processar e estamos rodando
    // e esperamos na condicional até sermos sinalizados que existe uma nova task
    while (empty(pool) && !pool->shutdown) {  
      pthread_cond_wait(&pool->task_notify, &pool->queue_lock); 
    } 

    //verifica se temos o sinal para desligar
    if (pool->shutdown == 1) { 
      pthread_mutex_unlock(&pool->queue_lock); 
      pthread_exit(NULL); 
    }

    //aloca a primeira task disponivel 
    task = pool->tasks + pool->head;

    //define o próximo head
    pool->head = (pool->head + 1) % pool->tasks_queue_size;
 
    //Como todas as threads são sinalizadas que existe uma task disponivel
    //é possivel que nesse momento as tasks disponiveis já foram processadas por
    //alguma outra thread e não existe mais tasks na fila.
    //Caso isso aconteça, espera na condicional
    if (empty(pool)) { 
      pthread_cond_signal(&pool->queue_empty); 
    }
 
    pthread_mutex_unlock(&pool->queue_lock);   
 
    (*(task->routine))(task->arg); 
  } 
}

//Função auxiliar para comparar head e tail
int empty(threadpool_t pool) { 
  return pool->head == pool->tail; 
}
