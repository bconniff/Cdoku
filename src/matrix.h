#ifndef DANCING_LINKS_H_GUARD
#define DANCING_LINKS_H_GUARD

typedef struct matrix matrix;
typedef struct node node;

matrix *new_matrix(unsigned w);
void free_matrix(matrix *m);
void matrix_add_row(matrix *m, void *r, unsigned pos[], unsigned len);
void **matrix_solve(matrix *m, int *len);

#endif
