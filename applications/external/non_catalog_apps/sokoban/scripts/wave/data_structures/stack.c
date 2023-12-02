#include "stack.h"
#include <stdlib.h>
#include <furi.h>

typedef struct StackNode
{
    void* value;
    struct StackNode* next;
} StackNode;

typedef struct Stack
{
    StackNode* head;
    int count;
} Stack;

Stack* stack_alloc()
{
    Stack* stack = malloc(sizeof(Stack));
    stack->head = NULL;
    stack->count = 0;
    return stack;
}

void stack_free(Stack* stack)
{
    furi_assert(stack->count == 0);
    free(stack);
}

void stack_push(Stack* stack, void* value)
{
    StackNode* node = malloc(sizeof(StackNode));
    node->value = value;
    node->next = stack->head;
    stack->head = node;
    stack->count++;
}

void* stack_pop(Stack* stack)
{
    if (stack->head == NULL)
        return NULL;

    StackNode* node = stack->head;
    stack->head = node->next;
    stack->count--;

    void* value = node->value;
    free(node);

    return value;
}

void* stack_peek(Stack* stack)
{
    if (stack->head == NULL)
        return NULL;

    return stack->head->value;
}

int stack_count(Stack* stack)
{
    return stack->count;
}

void* stack_discard_bottom(Stack* stack)
{
    if (stack->count <= 1)
        return stack_pop(stack);

    StackNode* newLast = stack->head;

    for (int i = 0; i < stack->count - 2; i++)
        newLast = newLast->next;

    StackNode* last = newLast->next;
    furi_assert(last != NULL);
    furi_assert(last->next == NULL);

    newLast->next = NULL;
    stack->count--;

    void* value = last->value;
    free(last);
    return value;
}