#include <stdlib.h>
#include "stack.h"

struct stack_node {
   void *item;
   struct stack_node *prev;
};

struct stack {
   stack_node *top;
   unsigned size;
};

stack *new_stack(void) {
   stack *s = malloc(sizeof(stack));

   s->top = NULL;
   s->size = 0;

   return s;
}

void stack_push(stack *s, void *item) {
   stack_node *n = malloc(sizeof(stack_node));

   n->prev = s->top;
   n->item = item;

   s->size++;
   s->top = n;
}

void *stack_pop(stack *s) {
   stack_node *tmp = s->top;
   void *item = tmp->item;

   s->top = tmp->prev;
   s->size--;

   free(tmp);

   return item;
}

void free_stack(stack *s) {
   while (s->size)
      stack_pop(s);

   free(s);
}

int stack_size(stack *s) {
   return s->size;
}
