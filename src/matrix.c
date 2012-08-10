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

/* represents a node in the DLX matrix */
struct node {
   void *r;
   struct node *prev, *next, *up, *down, *head;
};

/* represents the DLX matrix itself */
struct matrix {
   node **row; /* nodes at the bottom of each column */
   node *root; /* DLX root note */
};

/* picks the column with the minimum number of nodes */
node *get_col(matrix *m) {
   node *result = NULL;
   int count = -1, c;

   node *head, *n;
   for (head = m->root->next; head != m->root; head = head->next) {
      /* count the nodes in the column */
      c = 0;
      for (n = head->down; n != head; n = n->down, c++);

      /* if there are no nodes, pick the column */
      if (!c)
         return head;

      /* otherwise, test it against the current minimum */
      if (count == -1 || c < count) {
         count = c;
         result = head;
      }
   }

   /* return what we found */
   return result;
}

/* eliminates a column from the matrix in such a way that it can be easily
 * re-inserted later */
void cover_col(node *head) {
   /* eliminate the column from the header row */
   head->prev->next = head->next;
   head->next->prev = head->prev;

   /* eliminate all rows that the column contains an element in */
   node *x, *y;
   for (y = head->down; y != head; y = y->down) {
      for (x = y->next; x != y; x = x->next) {
         x->up->down = x->down;
         x->down->up = x->up;
      }
   }
}

void uncover_col(node *head) {
   /* re-insert the column's header node */
   head->prev->next = head;
   head->next->prev = head;

   /* re-insert all rows that the column contains an element in */
   node *x, *y;
   for (y = head->down; y != head; y = y->down) {
      for (x = y->next; x != y; x = x->next) {
         x->up->down = x;
         x->down->up = x;
      }
   }
}

/* converts the solution stack into a list of objects */
void **get_solution(stack *s, int *len) {
   /* get the stack size and allocate our list */
   const unsigned sz = stack_size(s);
   void **list = xmalloc(sz*sizeof(void*));
   *len = sz;

   /* pop items from the stack into the list until the stack is empty */
   int i = 0;
   while (stack_size(s)) {
      node *n = stack_pop(s);
      node *x;

      /* while we're at it, transform the matrix back to it's original state
       * so we can easily free it later */
      uncover_col(n->head);
      for (x = n->next; x != n; x = x->next)
         uncover_col(x->head);

      /* insert the item into the list */
      list[i++] = n->r;
   }

   return list;
}

/* the actual solver function, which is wrapped by matrix_solve */
void **matrix_solve_helper(matrix *m, stack *solution, int *len) {
   /* if the root node is the only node left, we're done */
   if (m->root == m->root->next)
      return get_solution(solution, len);

   /* pick a column and eliminate it */
   node *head = get_col(m);
   cover_col(head);

   /* try each row covered by the column picked */
   node *n, *x;
   for (n = head->down; n != head; n = n->down) {
      /* try erasing all the columns covered by the row we picked */
      for (x = n->next; x != n; x = x->next)
         cover_col(x->head);

      /* add the row into the solution for now... */
      stack_push(solution, n);

      /* try to solve the modified matrix */
      void **list;
      if (list = matrix_solve_helper(m, solution, len))
         return list;

      /* that didn't work... now we backtrack */
      stack_pop(solution);

      /* add everything we erased back into the matrix; this is easy since
       * we kept track of the node we selected */
      for (x = n->next; x != n; x = x->next)
         uncover_col(x->head);
   }

   /* no possible solutions for the column we picked... add it back into the
    * matrix and return NULL for failure */
   uncover_col(head);

   return NULL;
}

/* constructs a new DLX matrix with the given width */
matrix *new_matrix(unsigned w) {
   /* allocate matrix object and the list of nodes */
   matrix *m = xmalloc(sizeof(matrix));
   node **row = xmalloc(w * sizeof(node*));

   /* allocate the first node in the header row */
   node *first = xmalloc(sizeof(node));
   node *curr = first, *prev = first;

   /* allocate the header row */
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

   /* finish linking things up */
   prev->next = curr;
   curr->prev = prev;
   first->prev = curr;
   curr->next = first;

   /* set the root node to it's proper place */
   m->root = curr;

   /* set the current row of nodes to the header */
   m->row = row;

   return m;
}

/* inserts a row into the matrix with an associated object */
void matrix_add_row(matrix *m, void *r, unsigned pos[], unsigned len) {
   node *first = xmalloc(sizeof(node));
   first->r = r;

   node *curr = first, *prev = curr;

   /* for each position, allocate a new node and attach it to all it's
    * neighbors */
   int i;
   for (i = 0; i < len; i++) {
      /* this is sort of voodoo but it works perfectly */
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

   /* XXX this is sloppy, how about we just dont xmalloc it at all... */
   free(curr);

   /* finish linking things up */
   prev->next = first;
   first->prev = prev;
}

/* wraps the matrix_solve_helper function to solve the exact cover problem
 * represented by the DLX matrix */
void **matrix_solve(matrix *m, int *len) {
   stack *s = new_stack();
   void **soln = matrix_solve_helper(m, s, len);
   free_stack(s);
   return soln;
}

/* frees a matrix object */
void free_matrix(matrix *m) {
   /* free the list of nodes at the bottom of the columns */
   free(m->row);

   /* loop over the headers */
   node *head = m->root->next;
   while (head != m->root) {
      node *next = head->next;

      /* free all the nodes in the column */
      node *n = head->down;
      while (n != head) {
         node *down = n->down;
         free(n);
         n = down;
      }

      /* free the header */
      free(head);

      head = next;
   }

   /* free the root node and the matrix itself */
   free(m->root);
   free(m);
}
