#ifndef STACK_H_GUARD
#define STACK_H_GUARD

typedef struct stack_node stack_node;
typedef struct stack stack;

stack *new_stack(void);
void free_stack(stack *s);
void stack_push(stack *s, void *item);
void *stack_pop(stack *s);

#endif
