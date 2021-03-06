#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  FILE *fp;
  char *ptr;
  int i;
  float randomNumber;

  /* size of array */
  int size = strtol(argv[1], &ptr, 10);
  /* upper bound of array's random value */
  int upper = strtol(argv[2], &ptr, 10);

  printf("Generate random number array size (%d x 1)\n", size);
  printf("Range of array's value is between 0 - %d\n", upper);

  printf("Begin write file array.txt ..\n");

  fp = fopen("array.txt", "w");
  if (fp == NULL)
  {
    printf("Error open file ..\n");
    exit(1);
  }

  /* Write size of array in 1st line */
  fprintf(fp, "%d\n", size);

  /* make random unique */
  srand((unsigned int)time(NULL));

  /* Write rest of array to the file */
  for (i = 0; i < size; i++)
  {
    /* random number between the range upper - lower */
    randomNumber = ((float)rand() / (float)RAND_MAX) * upper;
    fprintf(fp, "%.1f\n", randomNumber);
  }

  /* close file */
  fclose(fp);

  printf("Write array.txt finished ..\n");

  return (0);
}
