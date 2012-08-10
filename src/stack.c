/* Copyright (c) 2012, Brendan Conniff
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of Brendan Conniff nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include "xmalloc.h"
#include "stack.h"

/* item in a stack, with a corresponding data object */
struct stack_node {
   void *item;
   struct stack_node *prev;
};

/* stack itself, which holds the top node and it's size */
struct stack {
   stack_node *top;
   unsigned size;
};

/* allocates a new stack */
stack *new_stack(void) {
   stack *s = xmalloc(sizeof(stack));

   /* empty... for now */
   s->top = NULL;
   s->size = 0;

   return s;
}

/* place an item on top of the stack */
void stack_push(stack *s, void *item) {
   stack_node *n = xmalloc(sizeof(stack_node));

   /* set up new node */
   n->prev = s->top;
   n->item = item;

   /* replace top node and increase size */
   s->size++;
   s->top = n;
}

/* removes the item on top of the stack */
void *stack_pop(stack *s) {
   /* get the top item and it's data object */
   stack_node *tmp = s->top;
   void *item = tmp->item;

   /* replace the top node with the one under and decrease size */
   s->top = tmp->prev;
   s->size--;

   /* free the old top node and return the date */
   free(tmp);
   return item;
}

/* deallocates a stack */
void free_stack(stack *s) {
   /* pop everything off and free */
   while (s->size)
      stack_pop(s);
   free(s);
}

/* retrieve the size of the stack */
int stack_size(stack *s) {
   return s->size;
}
