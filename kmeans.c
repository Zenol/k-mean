#include "kmeans.h"

#include <stdlib.h>
#include <stdio.h>

s_kmeans *kmeans_create(int ik, int iw, int ih)
{
    s_kmeans *km;
    int i;

    km = malloc(sizeof(*km));
    km->k = ik;
    km->mu = malloc(sizeof(*km->mu) * ik);

    for (i = 0; i < ik; i++)
        km->mu[i] = matrix_randomize(matrix_create(iw, ih));

    return km;
}

//Find the index i of the closer mu[i] to the matrix current
int find_closer(s_kmeans *km, s_matrix *current)
{
    int dist, new_dist;
    int idx, j;

    idx = 0;
    dist = matrix_dist(current, km->mu[0]);
//    printf("dist : %d\n", dist);
    for (j = 1; j < km->k; j++)
    {
        new_dist = matrix_dist(km->mu[j], current);
//        printf("new_dist : %d\n", new_dist);
        if (new_dist < dist)
        {
            dist = new_dist;
            idx = j;
        }
    }

    return idx;
}

void compute_asso_list(s_kmeans *km, int set_size, int *asso_id, s_matrix **data_set)
{
    int i;
//    printf("Asso list :\n");
    for (i = 0; i < set_size; i++)
    {
        asso_id[i] = find_closer(km, data_set[i]);
//        printf("Image %d is close to %d\n", i, asso_id[i]);
    }
}

void asso_swap(int **a, int **b)
{
    int *c;

    c = *a;
    *a = *b;
    *b = c;
}

// Check that ou mean is under 255 (should always be true)
unsigned char kmeans_truncate(unsigned int v)
{
    if (v > 255)
    {
        fprintf(stderr, "Overflow : %d", v);
        return 255;
    }
    return (unsigned char)v;
}

// Compute new barycenters from data set
void compute_means(s_kmeans *km, int set_size, s_matrix **data_set, int *asso_list)
{
    //TODO In case of a biiiiig data_set, the sum can overflow :s
    int i, j;
    int size;
    unsigned int **mu_sum;
    unsigned int *count;

    // Used to count how much object in each group
    count = malloc(km->k * sizeof(*count));
    // Save the size of our matrices
    size = km->mu[0]->size;
    // Alocate km->k big matrices
    mu_sum = malloc(sizeof(*mu_sum) * km->k);
    for (i = 0; i < km->k; i++)
    {
        mu_sum[i] = malloc(sizeof(*mu_sum[i]) * size);
        count[i] = 0;
    }

    // Sum all data into those km->k big matrices
    for (i = 0; i < set_size; i++)
    {
        for (j = 0; j < size; j++)
            mu_sum[asso_list[i]][j] += data_set[i]->data[j];
        count[asso_list[i]]++;
    }

    //Compute the mean
    for (j = 0; j < size; j++)
        for (i = 0; i < km->k; i++)
        {
            if (count[i] == 0)
                count[i] = 1;
            km->mu[i]->data[j] = kmeans_truncate(mu_sum[i][j] / count[i]);
        }
}

s_kmeans *kmeans_learn_from_set(s_kmeans *km, int set_size, s_matrix **data_set, int steps)
{
    int *asso_id;
    int *new_asso_id;
    int step_id;

    //Initialise asso list
    asso_id = malloc(sizeof(*asso_id) * set_size);
    new_asso_id = malloc(sizeof(*asso_id) * set_size);
    compute_asso_list(km, set_size, asso_id, data_set);

    for (step_id = 1; step_id < steps; step_id++)
    {
        fprintf(stdout, "Step %d over %d\r", step_id, steps);
        fflush(stdout);

        compute_means(km, set_size, data_set, asso_id);

        compute_asso_list(km, set_size, new_asso_id, data_set);
        asso_swap(&asso_id, &new_asso_id);
    }
    compute_means(km, set_size, data_set, asso_id);

    return km;
}
