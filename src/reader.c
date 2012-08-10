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
#include <stdio.h>
#include "xmalloc.h"
#include "reader.h"

char *next_line(FILE *file, int *l) {
   if (feof(file)) {
      return NULL;
   }

   char *line = xmalloc(BUF_SIZE*sizeof(char));
   size_t lenmax = BUF_SIZE, len = 0;
   int c;

   for (;;) {
      c = fgetc(file);

      if (c == EOF || c == '\n')
         break;

      line[len++] = c;

      if (len == lenmax) {
         line = xrealloc(line, (lenmax *= 2));
      }
   }

   *l = len;
   line[len++] = '\0';
   return xrealloc(line, len);
}

int **next_puzzle(int k, FILE *file) {
   const int n = k*k, n2 = n*n;
   int len;
   char *line;
   
   if (!(line = next_line(file, &len)))
      return NULL;

   int x, y, c;
   int **puzzle = NULL;

   if (len == n2) {
      puzzle = xmalloc(n * sizeof(int*));
      for (x = 0; x < n; x++)
         puzzle[x] = xmalloc(n * sizeof(int));

      for (y = 0; y < n; y++) {
         for (x = 0; x < n; x++) {
            const int v = line[x+n*y] - '0';
            puzzle[x][y] = (v > 0 && v <= n) ? v : 0;
         }
      }
   }

   free(line);
   return puzzle;
}

void free_puzzle(const int k, int **puzzle) {
   const int n = k*k;
   int i;
   for (i = 0; i < n; i++)
      free(puzzle[i]);
   free(puzzle);
}
