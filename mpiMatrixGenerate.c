#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void randomNumberGenerator(int* numbuffer, int N)
{	
	for(int i = 0; i < N; ++i)
	{
		 *numbuffer = rand()%99;
		 ++numbuffer;
	}
}
int main( int argc, char *argv[] )
{
	  int numberOferrs = 0, error;
    int size, process_rank, *randomNumbersBuffer;
    
    int n[5]= {8, 16, 32, 64, 128};
		char nsize[30];
    
    MPI_File filehandler;
    MPI_Status status;    
    MPI_Comm comm;

    comm = MPI_COMM_WORLD;
    MPI_Init( &argc, &argv);
    
    for(int i = 0; i < 5; i++)
    	{
    				char* file_name = "file_";
						int Dimension = n[i];
			
			 			//Making the file
						sprintf(nsize,"%d.txt",Dimension);
						
						char* name_memory = malloc(strlen(file_name) + strlen(nsize) + 1); 
						strcpy(name_memory, file_name);
    				strcat(name_memory,nsize);
			
						// Using the MPI function for opening the file
		  			error = MPI_File_open(comm, name_memory, MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &filehandler);
		  			MPI_Offset offset = 0;
		  			if (error) MPI_Abort(MPI_COMM_WORLD, 911);

		  			MPI_Comm_size( comm, &size );
		  			MPI_Comm_rank( comm, &process_rank );
		  			srand(process_rank*time(NULL));
		  			if(process_rank == 0)
		  				{	
		  					MPI_Offset temp = 1;
		  					// First element is the size of the matrix
								error = MPI_File_write(filehandler, (n+i), temp, MPI_INT, &status);
		  				}
		  
		  					// Dividing the matrix into chunks
		  					MPI_Offset chunk_memory = (Dimension*Dimension/size);
		  
		  					// Memory allocation for chunks
		  					randomNumbersBuffer = (int*)malloc((int)chunk_memory * sizeof(int));
		  
		  					// Using a function to generate the Random Numbers
								randomNumberGenerator(randomNumbersBuffer, (int)chunk_memory);
			
								// Write to file:
		  					// Create the offset based on rank:
		  					offset = ((process_rank*chunk_memory)+1 )*sizeof(int); // start of the view for each processor
			
		  					MPI_File_set_view(filehandler, offset, MPI_INT, MPI_INT, "native", MPI_INFO_NULL );
		  					error = MPI_File_write_all(filehandler, randomNumbersBuffer, chunk_memory, MPI_INT, &status );
		  					if (error) { numberOferrs++; }
		  					free(randomNumbersBuffer);
		  					error = MPI_File_close( &filehandler);
		  					if (error) { numberOferrs++; }
    			
    			
    	}
    	
      MPI_Finalize();
      return 0;
    	
}
