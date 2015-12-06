#include "matrix.h"

typedef struct s_kmeans {
    int k;
    s_matrix **mu;
    unsigned int *labels;
} s_kmeans;

// Create a kmeans engine
// (w, h) is the size of all the matrices studied
s_kmeans *kmeans_create(int k, int w, int h);

// Learn from a given data set
s_kmeans *kmeans_learn_from_set(s_kmeans *km, int set_size, s_matrix **data_set, int steps);

// Give a label to each generated picture. The label is stored in the list km->labels
// (You should have called learn_from_set)
void kmeans_give_labels(s_kmeans *km, int set_size, unsigned char *labels, s_matrix **pics);

// Return the label the algorithm give to picture (give_labels should have be caled)
unsigned int kmeans_get_label_from_learned_kmeans(s_kmeans *km, s_matrix *picture);
