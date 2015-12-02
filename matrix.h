#ifndef MATRIX_H_
# define MATRIX_H_

typedef struct s_matrix {
    int w;
    int h;
    int size;
    unsigned char *data;
} s_matrix;

// Create an empty image
s_matrix *matrix_create(int w, int h);

// Initialise the image
s_matrix *matrix_initialize(s_matrix *m);

// Initialise with random values
s_matrix *matrix_randomize(s_matrix *m);

// Get a reference to the pixel located at (x, y)
unsigned char *matrix_at(s_matrix *m, int x, int y);

// Try to save the matrix into path as a PBM file.
// Return -1 if failed, 0 otherwise.
int matrix_save(s_matrix *m, char *path);

// Compute l'infty distance
// REQUIRE : a->w = b->w and a->h = b->h.
unsigned int matrix_dist(s_matrix *a, s_matrix *b);

#endif /* !MATRIX_H_ */
