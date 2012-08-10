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

/* associated with rows in the DLX matrix, each row object represents a
 * unique possible value for an x-y coordinate on the Sudoku board */
typedef struct row {
   int x, y, val;
} row;

/* struct containing the DLX matrix and associated data */
typedef struct solver {
   int n, k, x_off, y_off, b_off;
   stack *rows;
   matrix *m;
} solver;

/* constructs a solver object */
solver *new_solver(int k) {
   solver *s = xmalloc(sizeof(solver));

   /* determine the various offsets for values in the DLX matrix */
   const int n = k*k;
   const int x_off = n*n;
   const int y_off = x_off+x_off;
   const int b_off = y_off+x_off;

   /* store everything in the solver object */
   s->k = k;
   s->n = n;
   s->x_off = x_off;
   s->y_off = y_off;
   s->b_off = b_off;

   /* allocate a stack to keep track of the objects we insert into the
    * DLX matrix, so we can free them later */
   s->rows = new_stack();

   /* construct the actual DLX matrix */
   s->m = new_matrix(b_off+x_off);

   return s;
}

/* frees a solver object */
void free_solver(solver *s) {
   /* free the matrix and all of the row objects we've added to it */
   free_matrix(s->m);
   while (stack_size(s->rows))
      free(stack_pop(s->rows));

   /* free the stack of row objects and the solver itself */
   free_stack(s->rows);
   free(s);
}

void add_val(solver *s, int x, int y, int val) {
   const int b = s->k*(y/s->k) + (x/s->k); /* box number */

   /* four constraints */
   int data[4];
   data[0] = s->n*y + x;                /* one value per cell */
   data[1] = s->x_off + (s->n*x + val); /* each value once per column */
   data[2] = s->y_off + (s->n*y + val); /* each value once per row */
   data[3] = s->b_off + (s->n*b + val); /* each value once per box */

   /* construct a row object to identify what the DLX row represents */
   row *r = xmalloc(sizeof(row));

   r->x = x;
   r->y = y;
   r->val = val;

   /* insert the row into the matrix, and add it to our stack so we can free
    * it later */
   matrix_add_row(s->m, r, data, 4);
   stack_push(s->rows, r);
}

/* converts a Sudoku grid to a DLX matrix, solves the DLX matrix, and converts
 * the result back into a Sudoku grid */
int **solve(int k, int **vals) {
   /* create a new solver object */
   solver *s = new_solver(k);
   const int n = s->n;

   /* insert all the input values into the solver */
   int x, y, i;
   for (x = 0; x < n; x++) {
      for (y = 0; y < n; y++) {
         /* if we have a specified value, insert it as a single possibility;
          * otherwise, insert each value as a possibility for that cell */
         if (0 < vals[x][y] && vals[x][y] <= n)
            add_val(s, x, y, vals[x][y] - 1);
         else
            for (i = 0; i < n; i++)
               add_val(s, x, y, i);
      }
   }

   /* retrieve the solution */
   int len;
   row **result = (row**)matrix_solve(s->m, &len);
   int **solution = NULL;

   /* check if the solver was successful */
   if (result) {
      /* we were successful, allocate a solution grid */
      solution = xmalloc(n*sizeof(int*));
      for (i = 0; i < n; i++)
         solution[i] = xmalloc(n*sizeof(int));

      /* insert the values into the solution grid */
      for (i = 0; i < len; i++) {
         /* each row object contains the x-y coordinates and value of a cell */
         row *r = result[i];
         solution[r->x][r->y] = r->val;
      }
   }

   /* free the list of row objects and the solver objects */
   free(result);
   free_solver(s);

   return solution;
}
