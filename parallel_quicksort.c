// #include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void swap(float *i, float *j)
{
  *i = *i + *j;
  *j = *i - *j;
  *i = *i - *j;
}

int partition(float *arr, int low, int high)
{
  int pivot = *(arr + high);

  int i = low - 1;
  int j;

  for (j = low; j <= high - 1; j++)
  {
    if (*(arr + j) <= pivot)
    {
      i++;
      swap(arr + i, arr + j);
    }
  }
  swap(arr + i + 1, arr + high);

  return i + 1;
}

void quicksort(float *arr, int low, int high)
{
  if (low < high)
  {
    int pivot = partition(arr, low, high);
#pragma omp parallel sections
    {
#pragma omp section
      quicksort(arr, low, pivot - 1);
#pragma omp section
      quicksort(arr, pivot + 1, high);
    }
  }
}

int main(int argc, char **argv)
{
  int numProc;
  int id;

  FILE *fp;

  /* Full array */
  int size;
  float *array;

  /* Sub array */
  int *sub_array_size;
  float *sub_array;

  int *displacement;

  /* Recieve */
  int receive_size;
  int *receive_displacement;

  int i, j, k;

  int numthread;

  double startTime, endTime;

  /* Set thread number */
  numthread = atoi(argv[3]);
  omp_set_num_threads(numthread);

  /* initialize MPI */
  // MPI_Init(&argc, &argv);
  // MPI_Comm_size(MPI_COMM_WORLD, &numProc);
  // MPI_Comm_rank(MPI_COMM_WORLD, &id);

  /* Allocate all variable */
  displacement = (int *)malloc(sizeof(int) * numProc);
  receive_displacement = (int *)malloc(sizeof(int) * numProc);
  sub_array_size = (int *)malloc(sizeof(int) * numProc);
  

  if (id == 0)
  {
    /* Read input file */
    fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
      printf("Cannot open %s !!", argv[1]);
      exit(0);
    }
    printf("Begin read %s .. \n", argv[1]);
    /* get size of array */
    fscanf(fp, "%d\n", &size);

    /* allocate array */
    array = (float *)malloc(sizeof(float) * size);

    /* looping read array's data */
    for (i = 0; i < size; i++)
      fscanf(fp, "%f\n", array + i);

    printf("Read %s success ..\nTime start counting ..\n", argv[1]);
    /* start counting time */
    startTime = MPI_Wtime();

    /***************************/
    /*     PARTITION ARRAY     */
    /***************************/

    /* Partitioning array */
    for (i = 0; i < numProc; i++)
      *(sub_array_size + i) = size / numProc;

    /* extras size */
    int extras = size % numProc;
    for (i = 0; i < extras; i++)
      *(sub_array_size + numProc - 1 - i)++;

    /* set displacement */
    *displacement = 0;
    *receive_displacement = size;
    for (i = 1; i < numProc; i++)
    {
      *(displacement + i) = *(displacement + i - 1) + *(sub_array_size + i - 1);
      *(receive_displacement + i) = *(receive_displacement + i - 1) + *(sub_array_size + i - 1);
    }
  }

  /***************************/
  /*      SCATTER ARRAY      */
  /***************************/

  /* Send size of partial array first for allocating receiving array */
  MPI_Scatter(sub_array_size, 1, MPI_INT, &receive_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
  /* Allocating array for recieving data from rank 0 */
  sub_array = (float *)malloc(sizeof(float) * receive_size);

  /* Scatter the array to other ranks */
  MPI_Scatterv(array, sub_array_size, displacement, MPI_FLOAT, sub_array, receive_size, MPI_FLOAT, 0, MPI_COMM_WORLD);

  /***************************/
  /*         SORTING         */
  /***************************/
  /* Call quicksort method */
  quicksort(sub_array, 0, receive_size-1);



  /* Gather all sub array */
  MPI_Gatherv(sub_array, &receive_size, MPI_FLOAT, array, &size, displacement, MPI_FLOAT, 0, MPI_COMM_WWORLD);
  
  if (id == 0)
  {
    /* Merge array */
    // for (i=0; i<size; i++)
    // {
    //   if ()
    // }
  }

}
