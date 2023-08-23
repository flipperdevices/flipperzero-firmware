typedef struct Queue Queue;

void queue_enqueue(Queue* queue, void* item);
void queue_dequeue(Queue* queue, void* item);
void queue_peek(Queue* queue, void* item);
int queue_count(Queue* queue);
int queue_capacity(Queue* queue);
void queue_clear(Queue* queue);
Queue* queue_alloc(int capacity, int elemSize);
void queue_free(Queue* queue);