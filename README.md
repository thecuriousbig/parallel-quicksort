# parallel-quicksort
parallel quicksort in C language by using OpenMP and MPI 

## Usage
### generate_array.c
for generate a array for testing a parallel quicksort program
#### Complie
```
gcc -o [exe_file_name] generate_array.c
```
Example
```
gcc -o generate_array generate_array.c
```
#### Run
```
generate_array [size_of_array] [lower_bound_of_random_value] [upper_bound_of_random_value]
```
Example
```
generate_array 100 1 50
```
