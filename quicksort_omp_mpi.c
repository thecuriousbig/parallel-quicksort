#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <omp.h>

void merge(float *array, float *sorted_array, int size, int *displacement, int *sub_array_size, int numProc)
{
  int i, j;
  int minProcinTurn;
  float min_in_turn;
  float valueinTurn;
  int *index;
  int *end;
  index = (int *)malloc(sizeof(int) * numProc);
  end = (int *)malloc(sizeof(int) * numProc);
  for (i = 0; i < numProc; i++)
  {
    index[i] = displacement[i];
    if (i < numProc - 1)
      end[i] = displacement[i + 1];
    else
      end[i] = size;
  }
  /*merging*/
  for (i = 0; i < size; i++)
  {
    min_in_turn = FLT_MAX;
    minProcinTurn = -1;

    for (j = 0; j < numProc; j++)
    {
      if (index[j] < end[j])
      {
        valueinTurn = array[index[j]];

        if (valueinTurn <= min_in_turn)
        {
          min_in_turn = valueinTurn;
          minProcinTurn = j;
        }
      }
    }
    sorted_array[i] = min_in_turn;
    index[minProcinTurn]++;
  }
}

int median(float *arr, int low, int high)
{
  int med = (low + high) / 2;
  if ((arr[low] <= arr[med] && arr[med] <= arr[high]) || (arr[high] <= arr[med] && arr[med] <= arr[low]))
    return med;
  else if ((arr[med] <= arr[low] && arr[low] <= arr[high]) || (arr[high] <= arr[low] && arr[low] <= arr[med]))
    return low;
  else if ((arr[med] <= arr[high] && arr[high] <= arr[low]) || (arr[low] <= arr[high] && arr[high] <= arr[med]))
    return high;
}

void quicksort(float *arr, int first, int last)
{
  int i_pivot, left, right;
  float pivot, temp;
  if (first >= last)
    return; // no need to sort
  // otherwise select a pivot
  i_pivot = median(arr, first, last);
  pivot = arr[i_pivot];
  left = first;
  right = last;
  while (left <= right)
  {
    if (arr[left] > pivot)
    { // swap left element with right element
      temp = arr[left];
      arr[left] = arr[right];
      arr[right] = temp;
      if (right == i_pivot)
        i_pivot = left;
      right--;
    }
    else
      left++;
  }
  // place the pivot in its place (i.e. swap with right element)
  temp = arr[right];
  arr[right] = pivot;
  arr[i_pivot] = temp;
// sort two sublists in parallel;

/* The recursive steps in quicksort execution is implemented as separate tasks */
#pragma omp task
  quicksort(arr, first, (right - 1));
#pragma omp task
  quicksort(arr, (right + 1), last);
}

int main(int argc, char **argv)
{
  int numProc;
  int id;
  FILE *fp;
  int size;
  float *array;
  float *sorted_array;
  int *sub_array_size;
  float *sub_array;
  int *displacement;
  int receive_size;
  int *receive_displacement;
  int i, j, k;
  int numthread;
  double startTime, endTime;

  /* Set thread number */
  numthread = atoi(argv[3]);
  omp_set_num_threads(numthread);
  /* initialize MPI */
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProc);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  /* Allocate all variable */
  displacement = (int *)malloc(sizeof(int) * numProc);
  receive_displacement = (int *)malloc(sizeof(int) * numProc);
  sub_array_size = (int *)malloc(sizeof(int) * numProc);

  if (id == 0)
  {
    /* Read input file */
    fp = fopen(argv[1], "r");
    /* get size of array */
    fscanf(fp, "%d\n", &size);
    /* allocate array */
    array = (float *)malloc(sizeof(float) * size);
    sorted_array = (float *)malloc(sizeof(float) * size);
    /* looping read array's data */
    for (i = 0; i < size; i++)
      fscanf(fp, "%f\n", &array[i]);
    fclose(fp);
    /* start counting time */
    startTime = MPI_Wtime();
    printf("start sort\n");

    /* Partitioning array */
    for (i = 0; i < numProc; i++)
      sub_array_size[i] = size / numProc;
    /* extras size */
    int extras = size % numProc;
    for (i = 0; i < extras; i++)
      sub_array_size[numProc - 1 - i]++;
    /* set displacement */
    displacement[0] = 0;
    receive_displacement[0] = size;
    for (i = 1; i < numProc; i++)
    {
      displacement[i] = displacement[i - 1] + sub_array_size[i - 1];
      receive_displacement[i] = receive_displacement[i - 1] + sub_array_size[i - 1];
    }
  }

  /* Send size of partial array first for allocating receiving array */
  MPI_Scatter(sub_array_size, 1, MPI_INT, &receive_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
  /* Allocating array for recieving data from rank 0 */
  sub_array = (float *)malloc(sizeof(float) * receive_size);
  /* Scatter the array to other ranks */
  MPI_Scatterv(array, sub_array_size, displacement, MPI_FLOAT, sub_array, receive_size, MPI_FLOAT, 0, MPI_COMM_WORLD);

  /* Call quicksort method */
#pragma omp parallel
  {
#pragma omp single
    quicksort(sub_array, 0, receive_size - 1);
  }

  /* Gather all sub array */
  MPI_Gatherv(sub_array, receive_size, MPI_FLOAT, array, sub_array_size, displacement, MPI_FLOAT, 0, MPI_COMM_WORLD);

  if (id == 0)
  {
    /* Merge */
    merge(array, sorted_array, size, displacement, sub_array_size, numProc);
    /* Sort complete */
    endTime = MPI_Wtime();
    printf("\n Time is %lf ", endTime - startTime);
    fp = fopen(argv[2], "w");
    fprintf(fp, "%d\n", size);
    for (i = 0; i < size; i++)
      /* random number between the range upper - lower */
      fprintf(fp, "%.1f\n", sorted_array[i]);
  }
  MPI_Finalize();
  return 0;
}
