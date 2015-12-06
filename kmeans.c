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
    km->labels = malloc(km->k * sizeof(*km->labels));

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
    count = calloc(km->k, sizeof(*count));
    // Save the size of our matrices
    size = km->mu[0]->size;
    // Alocate km->k big matrices
    mu_sum = calloc(km->k, sizeof(*mu_sum));
    for (i = 0; i < km->k; i++)
    {
        mu_sum[i] = calloc(size, sizeof(*mu_sum[i]));
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
    fprintf(stdout, "Step %d over %d\n", steps, steps);
    return km;
}

unsigned int kmeans_get_closest_picture(s_kmeans *km, s_matrix *picture)
{
  unsigned int j;
  unsigned int kidx;
  unsigned int min_dist;
  unsigned int tmp_dist;

  kidx = 0;
  min_dist = matrix_dist(km->mu[0], picture);
  for (j = 1; j < km->k; j++)
    {
      tmp_dist = matrix_dist(km->mu[j], picture);
      if (tmp_dist < min_dist)
	{
	  kidx = j;
	  min_dist = tmp_dist;
	}
    }
  return kidx;
}

// Return the label the algorithm give to picture (give_labels should have be caled)
unsigned int kmeans_get_label_from_learned_kmeans(s_kmeans *km, s_matrix *picture)
{
  return km->labels[kmeans_get_closest_picture(km, picture)];
}

void kmeans_give_labels(s_kmeans *km, int set_size, unsigned char *labels, s_matrix **pics)
{
  int i;
  int digit;
  int k_idx;
  int vote_idx;
  int best_vote;
  unsigned int **sum_labels;

  sum_labels = calloc(km->k, sizeof(*sum_labels));
  for (i = 0; i < km->k; i++)
    sum_labels[i] = calloc(10, sizeof (*sum_labels[i]));

  for (i = 0; i < set_size; i++)
    {
      k_idx = kmeans_get_closest_picture(km, pics[i]);
      sum_labels[k_idx][labels[i]]++;
    }

  for (i = 0; i < km->k; i++)
    {
      vote_idx = 0;
      best_vote = sum_labels[i][0];
      for (digit = 1; digit < 10; digit++)
	if (best_vote < sum_labels[i][digit])
	  {
	    best_vote = sum_labels[i][digit];
	    vote_idx = digit;
	  }
      km->labels[i] = vote_idx;
    }

  free(sum_labels);
}
