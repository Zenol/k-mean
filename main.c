#include <stdlib.h>
#include <stdio.h>

#include "matrix.h"
#include "kmeans.h"

#define IMAGES "train-images-idx3-ubyte"
#define LABELS "train-labels-idx1-ubyte"
#define NB_ITEMS 6000
#define K        15
#define STEPS    1000

//#define SAVE_PICS

s_matrix **pics;
unsigned char *labels;

int main()
{
    s_kmeans *km;
    FILE *f_images;
    FILE *f_labels;
    int magic;
    int nb;
    int w, h;
    int i;

    // Labels :
    printf("Read labels...\n");
    f_labels = fopen(LABELS, "r");

    // Guess nb labels match nb images
    fread(&magic, 4, 1, f_labels);
    fread(&nb, 4, 1, f_labels);
    nb = NB_ITEMS;

    labels = malloc(sizeof(char) * nb);
    for (i = 0; i < nb; i++)
        fread(&labels[i], sizeof(char), 1, f_labels);

    fclose(f_labels);

    // Images :
    printf("Read images...\n");
    f_images = fopen(IMAGES, "r");

    // Guess nb labels match nb images
    fread(&magic, 4, 1, f_images);
    fread(&magic, 4, 1, f_images);
    fread(&w, 4, 1, f_images);
    fread(&h, 4, 1, f_images);
    w = 28;
    h = 28;

    pics = malloc(sizeof(*pics) * nb);
    for (i = 0; i < nb; i++)
    {
        pics[i] = matrix_create(w, h);
        fread(pics[i]->data, sizeof(char), pics[i]->size, f_images);
    }


#ifdef SAVE_PICS
    // Save all pictures
    printf("Save pictures...\n");
    for (i = 0; i < nb; i++)
    {
        char buff[256];

        sprintf(buff, "%d.pbm", i);
        matrix_save(pics[i], buff);
    }
#endif

    //Learn
    printf("Learn...\n");
    km = kmeans_create(K, w, h);

    kmeans_learn_from_set(km, nb, pics, STEPS);

    //Save images
    printf("Save...\n");
    for (i = 0; i < K; i++)
    {
        char buff[256];

        sprintf(buff, "mu_%d.pbm", i);
        matrix_save(km->mu[i], buff);
    }

    return 0;
}
