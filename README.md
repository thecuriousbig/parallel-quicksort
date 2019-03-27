# parallel-quicksort
parallel quicksort in C language by using OpenMP and MPI 

## Usage
### generate_array.c
for generate a array with a random floating point number.
Use for testing with parallel-quicksort program.
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
generate_array [size_of_array] [upper_bound_of_random_value]
```
Example
```
generate_array 100 1000
```
