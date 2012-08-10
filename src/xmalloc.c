#include <stdlib.h>
#include <stdio.h>

void *xmalloc(size_t sz) {
   void *data = NULL;
   if (!(data = malloc(sz))) {
      fprintf(stderr, "failed to malloc %Zu bytes, exiting\n",
              sz);
      exit(1);
   }
   return data;
}
