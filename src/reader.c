#include <stdlib.h>
#include <stdio.h>
#include "xmalloc.h"
#include "reader.h"

int **read_puzzle(const int k, const char *name) {
   const int n = k*k;

   FILE *file;

   if (file = fopen(name, "r")) {
      int c, x, y;
      int **puzzle = xmalloc(n * sizeof(int*));
      for (x = 0; x < n; x++)
         puzzle[x] = xmalloc(n * sizeof(int));

      x = 0, y = 0;
      while (y < n && (c = fgetc(file)) != EOF) {
         puzzle[x][y] = c - '0';
         if (x == n-1) {
            x = 0;
            y++;
         } else x++;
      }
      fclose(file);
      return puzzle;
   } else {
      return NULL;
   }

}
