#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    char const *name;
    float value;
}CSV;


int write_to_file(int count, CSV *data, char const *fileName)
{
    FILE *f = fopen(fileName, "a+");
    if (f == NULL) return -1;
    while (count-- > 0) {

        fprintf(f, "%s;%f\n", data->name, data->value);
        ++data;
    }
    fclose(f);
    return 0; 
}


int main(int argc, char** argv) {
    const int PING_PONG_LIMIT = 100;

    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size != 2) {
        fprintf(stderr, "world_size have to be at least %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    double t1, t2;
    
    int ping_pong_count = 0;
    int partner_rank = (world_rank + 1) % 2;
    
    double tab[1000];
    
    printf("world rank is: %d \n", world_rank);
    
/*
 * CZESC SYNCHRONICZNA
 */
    

    while (ping_pong_count < PING_PONG_LIMIT)
    {
        if (world_rank == ping_pong_count % 2)
        {
            ping_pong_count++;
            tab[0]=MPI_Wtime();
            tab[1]=ping_pong_count;
            MPI_Send(&tab, 2, MPI_DOUBLE, partner_rank, 0, MPI_COMM_WORLD);
            printf("\nPING\n");
        }
        else
        {
            t2 = MPI_Wtime();
            MPI_Recv(&tab, 2, MPI_DOUBLE, partner_rank, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            ping_pong_count = tab[1];
            printf("\nPONG %f\n",  tab[0]-t2);
 
            char str[] = "%d";
            char str2[PING_PONG_LIMIT];
            sprintf(str2,str,ping_pong_count);
             
            CSV wyniki;
            wyniki.name = str2;
            wyniki.value = tab[0]-t2;
             
            write_to_file(1,&wyniki,"wyniki_async.csv");
        }
 
    }


/*
 * CZESC ASYNCHRONICZNA
 */
    
    //http://aragorn.pb.bialystok.pl/~wkwedlo/PC3.pdf
    
    
//    while (ping_pong_count < PING_PONG_LIMIT)
//    {
//        if (world_rank == ping_pong_count % 2)
//        {
//            MPI_Request rq[2];
//            MPI_Status stattab[2];
//            
//            t1 = MPI_Wtime();
//            
//            ping_pong_count++;
//            
//            tab[0]=MPI_Wtime();
//            tab[1]=ping_pong_count;
//            
//            MPI_Isend(&tab, 2, MPI_DOUBLE, partner_rank, 0, MPI_COMM_WORLD,&rq[0]);
//            MPI_Irecv(&tab, 2, MPI_DOUBLE, partner_rank, 0,MPI_COMM_WORLD,&rq[1]);
//            MPI_Waitall(2,rq,stattab);
//            printf("\nPING count: %d\n", ping_pong_count);
//        }
//        else
//        {
//            MPI_Request rq[2];
//            MPI_Status stattab[2];
//            
//            
//            MPI_Isend(&tab, 2, MPI_DOUBLE, partner_rank, 0, MPI_COMM_WORLD,&rq[0]);
//            MPI_Irecv(&tab, 2, MPI_DOUBLE, partner_rank, 0,MPI_COMM_WORLD,&rq[1]);
//        
//            MPI_Waitall(2,rq,stattab);
//            
//            ping_pong_count = tab[1];
//            
//            t2 = MPI_Wtime();
//            printf("\nPONG count: %d %f\n", ping_pong_count, t2-tab[0]);
//            
//            char str[] = "%d";
//            char str2[PING_PONG_LIMIT];
//            sprintf(str2,str,ping_pong_count);
//            
//            CSV wyniki;
//            wyniki.name = str2;
//            wyniki.value = t2-tab[0];
//            
//            write_to_file(1,&wyniki,"wyniki_sync.csv");
//        }
//    }
    MPI_Finalize();
}
