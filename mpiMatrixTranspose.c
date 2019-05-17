#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <time.h>

// To be used later
int  n =  256;
int  m =  64;



void localTrans (int *a, int n)

{
  int i, j;
  int ij, ji, l;
  double tmp;
  ij = 0;
  l = -1;
  for (i = 0; i < n; i++)
    {
      l += n + 1;
      ji = l;
      ij += i + 1;
      for (j = i+1; j < n; j++)
	{
	  tmp = a[ij];
	  a[ij] = a[ji];
	  a[ji] = tmp;
	  ij++;
	  ji += n;
	}
    }
}


// Swap values
void swap(int *a, int *b){

   int temp = *a;
   *a = *b;
   *b = temp;
}

// Local Transpose
void local_Trans(int a[n][m]){

   int otherRow = 0;
   int otherCol = 0;

   for(int i = 0; i < n; i+=m){

     for(int j= 0; j < m; j+=m){

          for(int k = 0; k < m; k++){

               for(int l = 0; l < k; l++){

                    if(i!=j){

                            otherRow =  (i + l);
                            otherCol =  (j + k);

                    }
                    else{

                           otherRow = (l + j);
                           otherCol = (k + i);
                    }

                    swap(&a[k+i][l+j],&a[otherRow][otherCol]);

                  }

            }
       }
  }
}

int main(int argc, char** argv){

         /* Start timer */
            struct timeval start, end;
            gettimeofday(&start,NULL);
         /* Timer */
        
        // Input and Output files
        
        char *in = argv [1];
        char *out = argv[2];

        // setup
        int rank, numProcs;
        int inputError;
        int SIZE;
        int *sizeChecker;
        int *tempBuffer;
        int n;

        // Initialize MPI
        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

        // MPI setup
        MPI_File fh;
        MPI_Win win;
        MPI_Status status;
        MPI_Offset disp;
        MPI_Offset blockSize;

        // Open file
        inputError = MPI_File_open(MPI_COMM_WORLD, in, MPI_MODE_RDONLY,MPI_INFO_NULL,&fh);

        // File succesfully opened?

	 if(inputError){

                if (rank == 0) printf("Error! %s: Couldn't open file %s\n", argv[0], argv[1]);
                MPI_Finalize();
                exit(1);
        }

        // Get the size of the matrix
        sizeChecker = (int*)malloc(sizeof(int));
        MPI_File_read(fh, sizeChecker, 1, MPI_INT, &status);
        SIZE = sizeChecker[0];

       free(sizeChecker);

        n = (int)(SIZE/numProcs);

        // Condition the buffer for each rank
        blockSize = SIZE*n;
        disp = ((rank*blockSize)+1)*sizeof(int);
        tempBuffer = (int*)malloc(SIZE*n*sizeof(int));

        // Read the file into the buffer
        MPI_File_set_view(fh, disp, MPI_INT, MPI_INT,"native",MPI_INFO_NULL);
        MPI_File_read(fh,tempBuffer,blockSize,MPI_INT, &status);
        MPI_File_close(&fh);
  
  n = SIZE;
  m = blockSize/SIZE;
  int b = numProcs;

  // Transposition

  int inputMatrix[n][m];
  int outputMatrix[n][m];

  if(rank == 0)
  {
   // printf("Transposing\n");
  }

  if (numProcs != b)
   {
     if (rank == 0)
        printf ("Error, number of processes must be %d for %d x %d  divided matrix transposition of %d x %d \n", b,n,m,n,n);

      MPI_Finalize ();
      return 1;
    }

  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++)
      inputMatrix[i][j] = tempBuffer[j + i*m];

// Block Transpose
 MPI_Alltoall (&inputMatrix[0][0],m * m,MPI_INT,&outputMatrix[0][0],m * m, MPI_INT, MPI_COMM_WORLD);


 for (int i = 0; i < b; i++)
	 localTrans (&outputMatrix[i * m][0], m);

// Write results to file
  
  
  for(int i = 0; i < n; i++){
  
	  for(int j =0; j < m; j++){
                  
		  if(i != 0 && j != 0)
	      	     tempBuffer[j + i*m] = outputMatrix[i][j];
	  }
  }


  inputError = MPI_File_open(MPI_COMM_WORLD,out, MPI_MODE_WRONLY| MPI_MODE_CREATE, MPI_INFO_NULL, &fh);
  

    if(inputError){

                if (rank == 0) printf("Error! %s: Couldn't open file %s\n", argv[0], argv[2]);
                MPI_Finalize();
                exit(2);
        }

 
 
   MPI_File_set_view(fh, disp, MPI_INT, MPI_INT,"native",MPI_INFO_NULL);


  if(rank = 0)
  	MPI_File_write(fh, tempBuffer,blockSize, MPI_INT,&status);
  else
	  MPI_File_write_all(fh, tempBuffer,blockSize, MPI_INT,&status);

  /* Stop Timer */
  gettimeofday(&end, NULL);
 

  free(tempBuffer);

  MPI_File_close(&fh);

  if (rank == 0)
   // printf ("Transpose seems ok\n");

  MPI_Finalize ();

  /*Time duration*/

  float duration = (end.tv_sec -start.tv_sec) * 1e6;
  duration = (duration + (end.tv_sec -start.tv_sec)) * 1e-6;
  printf("Size of matrix : %d x %d\nt = %7f seconds \n", SIZE, SIZE, duration);
return 0;
}
