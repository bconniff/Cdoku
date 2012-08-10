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

#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
#include "solver.h"

#define CONST_K 3
#define CONST_N 9

void solve_file(char *name) {
   FILE *file;
   int x, y;
   if (file = fopen(name, "r")) {
      printf("Reading from file: %s\n", name);
      unsigned i = 0;
      for (;;) {
         int **puzzle = next_puzzle(CONST_K, file);

         if (feof(file))
            break;

         printf("   Solving puzzle #%u: ", ++i);
         if (puzzle) {
            int **soln = solve(CONST_K, puzzle);
            if (soln) {
               printf("Solved.\n");
               for (y = 0; y < CONST_N; y++) {
                  printf("      ");
                  for (x = 0; x < CONST_N; x++)
                     printf("%x", soln[x][y]);
                  printf("\n");
               }
               free_puzzle(CONST_K, soln);
            } else {
               printf("No solution.\n");
            }
            free_puzzle(CONST_K, puzzle);
         } else if (feof(file)) {
            break;
         } else {
            printf("Invalid length.\n");
         }
      }
      fclose(file);
   } else {
      printf("Couldn't open file: %s\n", name);
   }
}

int main(int argc, char **argv) {
   int i;
   if (argc > 1) {
      for (i = 1; i < argc; i++) {
         solve_file(argv[i]);
      }
   } else {
      printf("cdoku - DLX Sudoku Solver in C\n");
      printf("usage: %s [file]...\n", argv[0]);
   }
   return 0;
}
