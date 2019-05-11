#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{

	char array[10];
	int p_rank;
	char message[100];

  MPI_Offset n = 10;
  MPI_File filehandler; MPI_Status status;
  
  MPI_Init(&argc, &argv);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &p_rank);
  if(p_rank !=0)
  {
  	sprintf(message, "Hello %d", p_rank);
  	printf("%s\n", message);
  	
  }
  
  for (int i=0;i<10;i++) array[i] = (char)('0'+p_rank);
  
  MPI_File_open (MPI_COMM_WORLD, "data.out", MPI_MODE_CREATE|MPI_MODE_WRONLY,MPI_INFO_NULL, &filehandler);
  MPI_Offset space = p_rank*n*sizeof(char);
  
  MPI_File_set_view (filehandler, space, MPI_CHAR, MPI_CHAR, "native", MPI_INFO_NULL);
  
  MPI_File_write(filehandler,array,n,MPI_CHAR,&status);
  MPI_File_close(&filehandler);
  MPI_Finalize();
  
  return 0;
  
 } 
  
