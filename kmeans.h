#include "matrix.h"

typedef struct s_kmeans {
    int k;
    s_matrix **mu;
} s_kmeans;

// Create a kmeans engine
// (w, h) is the size of all the matrices studied
s_kmeans *kmeans_create(int k, int w, int h);

// Learn from a given data set
s_kmeans *kmeans_learn_from_set(s_kmeans *km, int set_size, s_matrix **data_set, int steps);
