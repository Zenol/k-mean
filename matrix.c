#include "matrix.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

s_matrix *matrix_create(int iw, int ih)
{
    s_matrix *m;

    m = malloc(sizeof(struct s_matrix));
    m->w = iw;
    m->h = ih;
    m->size = iw * ih;
    m->data = malloc(m->size * sizeof(char*));

    return m;
}

s_matrix *matrix_initialize(s_matrix *m)
{
    int i;

    for (i = 0; i < m->size; i++)
        m->data[i] = 0;

    return m;
}

s_matrix *matrix_randomize(s_matrix *m)
{
    int i;

    for (i = 0; i < m->size; i++)
        m->data[i] = rand() % 256;

    return m;
}

unsigned char *matrix_at(s_matrix *m, int x, int y)
{
    return &m->data[x + m->w * y];
}

int matrix_save(s_matrix *m, char *path)
{
    FILE *f;
    int i, j;

    f = fopen(path, "w+");
    if (!f)
        return -1;

    fprintf(f, "P2\n");
    fprintf(f, "%d %d\n", m->w, m->h);
    fprintf(f, "255\n");

    for (i = 0; i < m->w; i++)
    {
        for (j = 0; j < m->h; j++)
        {
            fprintf(f, "%d ", m->data[i + j * m->w]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
    return 0;
}

unsigned int matrix_dist(s_matrix *a, s_matrix *b)
{
    int i;
    unsigned int diff;
    unsigned int tmp;

    diff = 0;
    for (i = 0; i < a->size; i++)
    {
        tmp = abs(a->data[i] - b->data[i]);
	diff += tmp*tmp;
    }

    diff = floor(sqrt(diff));
    return diff;
}
