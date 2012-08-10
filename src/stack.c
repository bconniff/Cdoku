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

struct stack_node {
   void *item;
   struct stack_node *prev;
};

struct stack {
   stack_node *top;
   unsigned size;
};

stack *new_stack(void) {
   stack *s = xmalloc(sizeof(stack));

   s->top = NULL;
   s->size = 0;

   return s;
}

void stack_push(stack *s, void *item) {
   stack_node *n = xmalloc(sizeof(stack_node));

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
