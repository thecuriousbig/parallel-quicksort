#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

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
  int i, j, k;
  int numthread;
  double starttime, endtime;

  /* initialize MPI */
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProc);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  /* Set thread number */
  numthread = atoi(argv[3]);
  omp_set_num_threads(numthread);

  /* Read input file */
  fp = fopen(argv[1], "r");
  /* get size of array */
  fscanf(fp, "%d\n", &size);
  /* allocate array */
  array = (float *)malloc(sizeof(float) * size);
  /* looping read array's data */
  for (i = 0; i < size; i++)
    fscanf(fp, "%f\n", &array[i]);
  fclose(fp);
  /* start counting time */
  starttime = MPI_Wtime();
  printf("start sort\n");

  /* Call quicksort method */
#pragma omp parallel
  {
#pragma omp single
    quicksort(array, 0, size - 1);
  }

  endtime = MPI_Wtime();
  printf("Sort complete : %lf\n", endtime - starttime);

  fp = fopen(argv[2], "w");
  fprintf(fp, "%d\n", size);
  for (i = 0; i < size; i++)
  {
    /* random number between the range upper - lower */
    fprintf(fp, "%.1f\n", array[i]);
  }
  MPI_Finalize();
  return 0;
}
