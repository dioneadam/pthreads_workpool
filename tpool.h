typedef struct tpool_work { 
  void (*routine)(void *); 
  void *arg; 
  struct tpool_work *next; 
} tpool_work_t; 
 
typedef struct tpool { 
  int threads_quantity;         // Numero de threads
  int tasks_queue_size;         // Tamanho da fila de tarefas 
  pthread_t *threads;           // Array de threads
  tpool_work_t *tasks;          // Array de tarefas
  int head;                     // Index primeira tarefa
  int tail;                     // Index ultima tarefa
  int queue_closed;             // The remaining tasks can be completed, but no new tasks can be added
  int shutdown;                 // The rest of the task will not be done. Close it directly
  pthread_mutex_t queue_lock;   
  pthread_cond_t task_notify; 
  pthread_cond_t can_add_task; 
  pthread_cond_t queue_empty; 
} *tpool_t; 
 
void tpool_init(tpool_t *tpoolp,int threads_quantity, int tasks_queue_size); 
 
int tpool_add_work(tpool_t tpool,void(*routine)(void *), void *arg); 
 
int tpool_destroy(tpool_t tpool,int finish);
