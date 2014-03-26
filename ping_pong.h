#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    char const *name;
    float value;
}CSV;


#ifdef MPI_WTIME_IS_GLOBAL
    #define MPI_WTIME_IS_GLOBAL 1
#endif



int write_to_file(int count, CSV *data, char const *fileName);
