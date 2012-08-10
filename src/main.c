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

/* solves all the Sudoku puzzles in the given file */
void solve_file(char *name) {
   FILE *file;
   int x, y;

   /* try to open the file */
   if (file = fopen(name, "r")) {
      printf("Reading from file: %s\n", name);
      unsigned i = 0;

      /* keep going until the file ends */
      for (;;) {
         /* try to get the next puzzle */
         int **puzzle = next_puzzle(CONST_K, file);

         /* if we hit an EOF, call it quits */
         if (feof(file))
            break;

         /* make sure the puzzle was valid */
         printf("   Solving puzzle #%u: ", ++i);
         if (puzzle) {
            int **soln;

            /* valid puzzle, try to solve it */
            if (soln = solve(CONST_K, puzzle)) {
               /* found a solution, print it out and clean up */
               printf("Solved.\n");

               for (y = 0; y < CONST_N; y++) {
                  printf("      ");
                  for (x = 0; x < CONST_N; x++)
                     printf("%x", soln[x][y]);
                  printf("\n");
               }

               free_puzzle(CONST_K, soln);
            } else {
               /* puzzle couldn't be solved */
               printf("No solution.\n");
            }

            /* clean up our mess */
            free_puzzle(CONST_K, puzzle);
         } else {
            /* puzzle wasn't valid... the only way this can happen is if the
             * puzzle was the wrong length */
            printf("Invalid length.\n");
         }
      }
      /* check fclose return value, just for good practice */
      if (fclose(file))
         printf("Failed to close file: %s\n", name);
   } else {
      printf("Couldn't open file: %s\n", name);
   }
}

/* main program */
int main(int argc, char **argv) {
   int i;

   /* check number of arguments */
   if (argc > 1) {
      /* solve the puzzles provided */
      for (i = 1; i < argc; i++)
         solve_file(argv[i]);
   } else {
      /* no arguments, print usage */
      printf("cdoku - DLX Sudoku Solver in C\n");
      printf("usage: %s [file]...\n", argv[0]);
   }

   return 0;
}
