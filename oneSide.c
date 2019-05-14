#include <stdio.h>
#include "mpi.h"


#define NUM_ELEMENT 4


int main(int argc, char** argv){

 int i, id, num_procs, len, localBuffer[NUM_ELEMENT], sharedBuffer[NUM_ELEMENT];

 char name[MPI_MAX_PROCESSOR_NAME];
 MPI_Win win;

 MPI_Init(&argc,&argv);
 MPI_Comm_rank(MPI_COMM_WORLD,&id);
 MPI_Comm_size(MPI_COMM_WORLD,&num_procs);

 printf("Rank %d running on %s\n", id, name);

 MPI_Win_create(sharedBuffer,NUM_ELEMENT, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

 for(i = 0; i < NUM_ELEMENT; i++) {
 
    sharedBuffer[i] = 10*id + i;
    localBuffer[i] = 0;
 
 }

 printf("Rank %d sets data in shared memory: " ,id);

 for(i = 0; i < NUM_ELEMENT; i++)
	 printf(" %02d", sharedBuffer[i]);

 printf("\n");


 MPI_Win_fence(0, win);

 if(id != 0)
	 MPI_Get(&localBuffer[0], NUM_ELEMENT, MPI_INT, id-1, 0, NUM_ELEMENT, MPI_INT, win);
 else
	 MPI_Get(&localBuffer[0], NUM_ELEMENT, MPI_INT, num_procs-1,0, NUM_ELEMENT, MPI_INT, win);

 MPI_Win_fence(0, win);
 
 printf("Rank %d gets data from shared memory: " , id);

 for(i = 0; i < NUM_ELEMENT; i++){
	 printf(" %02d", localBuffer[i]);
 }
 printf("\n");

 MPI_Win_fence(0,win);

 if(id < num_procs - 1)
	MPI_Put(&localBuffer[0], NUM_ELEMENT, MPI_INT, id+1, 0, NUM_ELEMENT, MPI_INT, win);
  else
    MPI_Put(&localBuffer[0], NUM_ELEMENT, MPI_INT, 0, 0, NUM_ELEMENT, MPI_INT, win);
 
 MPI_Win_fence(0, win);

 printf("Rank %d has new data in shared memory: ", id);

 for(i =0; i < NUM_ELEMENT; i++)
	 printf(" %02d", sharedBuffer[i]);
 
 printf("\n");

 MPI_Win_free(&win);
 MPI_Finalize();
 return 0;
}
