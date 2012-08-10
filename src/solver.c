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
#include "matrix.h"
#include "stack.h"

typedef struct row {
   int x, y, val;
} row;

typedef struct solver {
   int n, k, x_off, y_off, b_off;
   stack *rows;
   matrix *m;
} solver;

row *new_row(int x, int y, int val) {
   row *r = xmalloc(sizeof(row));

   r->x = x;
   r->y = y;
   r->val = val;

   return r;
}

solver *new_solver(int k) {
   solver *s = xmalloc(sizeof(solver));

   const int n = k*k;
   const int x_off = n*n;
   const int y_off = x_off+x_off;
   const int b_off = y_off+x_off;

   s->k = k;
   s->n = n;
   s->x_off = x_off;
   s->y_off = y_off;
   s->b_off = b_off;

   s->rows = new_stack();
   s->m = new_matrix(b_off+x_off);

   return s;
}

void free_solver(solver *s) {
   free_matrix(s->m);
   while (stack_size(s->rows))
      free(stack_pop(s->rows));
   free_stack(s->rows);
   free(s);
}

int b_num(int k, int x, int y) {
   return k*(y/k) + (x/k);
}

void add_val(solver *s, int x, int y, int val) {
   int data[4];

   data[0] = s->n*y + x;
   data[1] = s->x_off + (s->n*x + val);
   data[2] = s->y_off + (s->n*y + val);
   data[3] = s->b_off + (s->n*b_num(s->k,x,y) + val);

   row *r = new_row(x, y, val + 1);
   matrix_add_row(s->m, r, data, 4);
   stack_push(s->rows, r);
}

int **solve(int k, int **vals) {
   solver *s = new_solver(k);
   const int n = s->n;

   int x, y, i;
   for (x = 0; x < n; x++) {
      for (y = 0; y < n; y++) {
         if (0 < vals[x][y] && vals[x][y] <= n)
            add_val(s, x, y, vals[x][y] - 1);
         else
            for (i = 0; i < n; i++)
               add_val(s, x, y, i);
      }
   }

   int len;
   row **result = (row**)matrix_solve(s->m, &len);
   int **solution = NULL;

   if (result) {
      solution = xmalloc(n*sizeof(int*));
      for (i = 0; i < n; i++)
         solution[i] = xmalloc(n*sizeof(int));
      for (i = 0; i < len; i++) {
         row *r = result[i];
         solution[r->x][r->y] = r->val;
      }
   }

   free_solver(s);
   return solution;
}
