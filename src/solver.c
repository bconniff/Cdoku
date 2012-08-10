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
