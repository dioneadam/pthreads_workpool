//Nomes: Dione Adam, Julia Boesing
//Computação de Alto Desempenho 2022/01

// Struct para fila de tarefas
typedef struct threadpool_task { 
  void (*routine)(void *); 
  void *arg; 
  struct threadpool_task *next; 
} threadpool_task_t; 
 
//Struct para threadpool 
typedef struct threadpool { 
  int pool_size;                // Numero de threads
  int tasks_queue_size;         // Tamanho da fila de tarefas 
  pthread_t *threads;           // Fila de threads
  threadpool_task_t *tasks;     // Fila de tarefas
  int head;                     // Primeira tarefa
  int tail;                     // Ultima tarefa
  int queue_closed;             // As tarefas restantes podem ser completadas, mas não pode ser adicionada novas tarefas
  int shutdown;                 // Terminar imediatamente, mesmo se houver tarefas não finalizadas
  pthread_mutex_t queue_lock;   // Mutex
  pthread_cond_t task_notify;   // Condicional para notificar que existe uma nova tarefa
  pthread_cond_t queue_empty;   // COndicional para notificar que a fila  de tarefas está vazia
} *threadpool_t; 
 
void threadpool_create(threadpool_t *threadpool, int threads_quantity, int tasks_queue_size); 
 
int threadpool_add_task(threadpool_t threadpool, void(*routine)(void *), void *arg); 
 
int threadpool_destroy(threadpool_t threadpool, int finish);
