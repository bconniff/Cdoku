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
#include "matrix.h"

struct node {
   void *r;
   struct node *prev, *next, *up, *down, *head;
};

struct matrix {
   node **row;
   node *root;
};

node *get_col(matrix *m) {
   node *result = NULL;
   int count = -1, c;

   node *head, *n;
   for (head = m->root->next; head != m->root; head = head->next) {
      c = 0;
      for (n = head->down; n != head; n = n->down, c++);

      if (!c)
         return head;

      if (count == -1 || c < count) {
         count = c;
         result = head;
      }
   }

   return result;
}

void cover_col(node *head) {
   head->prev->next = head->next;
   head->next->prev = head->prev;

   node *x, *y;
   for (y = head->down; y != head; y = y->down) {
      for (x = y->next; x != y; x = x->next) {
         x->up->down = x->down;
         x->down->up = x->up;
      }
   }
}

void uncover_col(node *head) {
   head->prev->next = head;
   head->next->prev = head;

   node *x, *y;
   for (y = head->down; y != head; y = y->down) {
      for (x = y->next; x != y; x = x->next) {
         x->up->down = x;
         x->down->up = x;
      }
   }
}

void **get_solution(stack *s, int *len) {
   const unsigned sz = stack_size(s);
   void **list = xmalloc(sz*sizeof(void*));
   *len = sz;

   int i = 0;
   while (stack_size(s)) {
      node *n = stack_pop(s);
      node *x;

      uncover_col(n->head);
      for (x = n->next; x != n; x = x->next)
         uncover_col(x->head);

      list[i] = n->r;

      i++;
   }

   return list;
}

void **matrix_solve_helper(matrix *m, stack *solution, int *len) {
   if (m->root == m->root->next)
      return get_solution(solution, len);

   node *head = get_col(m);

   cover_col(head);

   node *n, *x;
   for (n = head->down; n != head; n = n->down) {
      for (x = n->next; x != n; x = x->next)
         cover_col(x->head);

      stack_push(solution, n);

      void **list;
      if (list = matrix_solve_helper(m, solution, len))
         return list;

      stack_pop(solution);

      for (x = n->next; x != n; x = x->next)
         uncover_col(x->head);
   }

   uncover_col(head);

   return NULL;
}

matrix *new_matrix(unsigned w) {
   matrix *m = xmalloc(sizeof(matrix));
   node **row = xmalloc(w*sizeof(node*));

   node *first = xmalloc(sizeof(node));
   node *curr = first, *prev = first;

   int i;
   for (i = 0; i < w; i++) {
      curr->prev = prev;
      prev->next = curr;
      curr->up = curr;
      curr->down = curr;
      curr->head = curr;
      row[i] = curr;
      prev = curr;
      curr = xmalloc(sizeof(node));
   }

   prev->next = curr;
   curr->prev = prev;

   first->prev = curr;
   curr->next = first;

   m->root = curr;
   m->row = row;

   return m;
}

void matrix_add_row(matrix *m, void *r, unsigned pos[], unsigned len) {
   node *first = xmalloc(sizeof(node));
   first->r = r;

   node *curr = first, *prev = curr;

   int i;
   for (i = 0; i < len; i++) {
      node *x = m->row[pos[i]];

      curr->prev = prev;
      prev->next = curr;
      curr->up = x;
      curr->down = x->down;
      curr->head = x->head;
      x->down = curr;
      curr->down->up = curr;

      m->row[pos[i]] = curr;
      prev = curr;
      curr = xmalloc(sizeof(node));
      curr->r = r;
   }

   free(curr);

   prev->next = first;
   first->prev = prev;
}

void **matrix_solve(matrix *m, int *len) {
   stack *s = new_stack();
   void **soln = matrix_solve_helper(m, s, len);
   free_stack(s);
   return soln;
}

void free_matrix(matrix *m) {
   free(m->row);

   node *head = m->root->next;
   while (head != m->root) {
      node *next = head->next;

      node *n = head->down;
      while (n != head) {
         node *down = n->down;
         free(n);
         n = down;
      }
      free(head);

      head = next;
   }
   free(m->root);

   free(m);
}
