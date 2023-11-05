#include "queue.h"
#include <stdlib.h>
#include <string.h>

struct Queue
{
    char* array;
    int front;
    int rear;
    int capacity;
    int elemSize;
};

void queue_enqueue(Queue* queue, void* item)
{
    void* dest = queue->array + queue->rear * queue->elemSize;
    memcpy(dest, item, queue->elemSize);
    queue->rear = (queue->rear + 1) % queue->capacity;
}

void queue_peek(Queue* queue, void* item)
{
    void* src = queue->array + queue->front * queue->elemSize;
    memcpy(item, src, queue->elemSize);
}

void queue_dequeue(Queue* queue, void* item)
{
    queue_peek(queue, item);
    queue->front = (queue->front + 1) % queue->capacity;
}

int queue_count(Queue* queue)
{
    return (queue->rear - queue->front + queue->capacity) % queue->capacity;
}

int queue_capacity(Queue* queue)
{
    return queue->capacity;
}

void queue_clear(Queue* queue)
{
    queue->front = queue->rear = 0;
}

Queue* queue_alloc(int capacity, int elemSize)
{
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->array = (char*)malloc(capacity * elemSize);
    queue->capacity = capacity;
    queue->elemSize = elemSize;
    queue_clear(queue);
    return queue;
}

void queue_free(Queue* queue)
{
    free(queue->array);
    free(queue);
}