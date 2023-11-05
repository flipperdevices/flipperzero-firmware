typedef struct Stack Stack;

Stack* stack_alloc();
void stack_free(Stack* stack);

void stack_push(Stack* stack, void* value);
void* stack_pop(Stack* stack);
void* stack_peek(Stack* stack);
void* stack_discard_bottom(Stack* stack);

int stack_count(Stack* stack);
int stack_capacity(Stack* stack);