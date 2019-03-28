// #include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  int numProc;
  int id;

  FILE *fp;

  int size;
  float *unsorted_array;
  float *sorted_array;
  int i, j, k;

  /* initialize MPI */
  // MPI_Init(&argc, &argv);
  // MPI_Comm_size(MPI_COMM_WORLD, &numProc);
  // MPI_Comm_rank(MPI_COMM_WORLD, &id);

  /* Read input file */
  fp = fopen(argv[1], "r");
  if (fp == NULL)
  {
    printf("Begin read %s file .. \n", argv[1]);
    exit(0);
  }
  fscanf(fp, "%d\n", &size);
  unsorted_array = (float *)malloc(sizeof(float) * size);
  for (i = 0; i < size; i++)
  {
    fscanf(fp, "%f\n", unsorted_array + i);
  }

  for (i = 0; i < size; i++)
    printf("%.1f\n", *(unsorted_array + i));
}
