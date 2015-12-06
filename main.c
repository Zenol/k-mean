#define _BSD_SOURCE
#include <endian.h>

#include <stdlib.h>
#include <stdio.h>


#include "matrix.h"
#include "kmeans.h"

#define IMAGES       "train-images-idx3-ubyte"
#define LABELS       "train-labels-idx1-ubyte"
#define IMAGES_CHECK "train-images-idx3-ubyte"
#define LABELS_CHECK "train-labels-idx1-ubyte"
#define K        15
#define STEPS    15

//#define SAVE_PICS

s_matrix **pics;
unsigned char *labels;

s_matrix **load_images(char *filename, int *nb, int *w, int *h)
{
  int magic;
  FILE *f_images;
  s_matrix **pics;
  int i;

  f_images = fopen(filename, "r");

  // Guess nb labels match nb images
  fread(&magic, 4, 1, f_images);
  fread(nb, 4, 1, f_images);
  fread(h, 4, 1, f_images);
  fread(w, 4, 1, f_images);
  magic = be32toh(magic);
  *nb = be32toh(*nb);
  *h = be32toh(*h);
  *w = be32toh(*w);

  pics = malloc(sizeof(*pics) * *nb);
  for (i = 0; i < *nb; i++)
    {
      pics[i] = matrix_create(*h, *w);
      fread(pics[i]->data, sizeof(char), pics[i]->size, f_images);
    }

  fclose(f_images);

  return pics;
}

unsigned char *load_labels(char *filename, int *nb)
{
  unsigned char *labels;
  FILE *f_labels;
  int magic;
  int i;
  
  f_labels = fopen(filename, "r");

  fread(&magic, 4, 1, f_labels);
  fread(nb, 4, 1, f_labels);
  magic = be32toh(magic);
  *nb = be32toh(*nb);

  labels = malloc(sizeof(char) * *nb);
  for (i = 0; i < *nb; i++)
    fread(&labels[i], sizeof(char), 1, f_labels);

  fclose(f_labels);

  return labels;
}

unsigned int check_algo(s_kmeans *km, unsigned int nb, unsigned char *labels, s_matrix **pics)
{
  int i;
  unsigned int count;

  count = 0;
  for (i = 0; i < nb; i++)
    {
      if(labels[i] == kmeans_get_label_from_learned_kmeans(km, pics[i]))
	count++;
    }
  return count;
}

int main()
{
    s_kmeans *km;
    int magic;
    int nb;
    int w, h;
    int i;
    int how_much_right;

    // Guess nb labels match nb images

    // Labels :
    printf("Read labels...\n");
    labels = load_labels(LABELS, &nb);

    // Images :
    printf("Read images...\n");
    pics = load_images(IMAGES, &nb, &w, &h);

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

    //Labelate all pictures
    kmeans_give_labels(km, nb, labels, pics);

    //Check efficiency with an other data set
    for (i = 0; i < nb; i++)
      free(pics[i]);
    free(pics);
    free(labels);
    
    labels = load_labels(LABELS_CHECK, &nb);
    pics = load_images(IMAGES_CHECK, &nb, &w, &h);
    
    how_much_right = check_algo(km, nb, labels, pics);

    printf("Efficiency : %d / %d = %f percents\n",
	   how_much_right, nb, (float)how_much_right / (float)nb * 100.f);

    //Save images
    printf("\nSave...\n");
    for (i = 0; i < K; i++)
    {
        char buff[256];

        sprintf(buff, "mu_k%d_d%d.pbm", i, km->labels[i]);
        matrix_save(km->mu[i], buff);
    }

    return 0;
}
