#include <stdio.h>
#include <mpi.h>

int  n =  256;
int  m =  64;


// Swap values
void swap(int *a, int *b){

   int temp = *a;
   *a = *b;
   *b = temp;
}

// Local Transpose
void localTrans(int a[n][m]){

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


int main (int argc, char *argv[])
{


  int b = n/m;

  int inputMatrix[n][m];
  int outputMatrix[n][m];

  int i, j, nprocs, rank;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);

  printf("%d\n", nprocs);

  if(rank == 0)
  {
    printf("Transposing\n");
  }

  if (nprocs != b)
   {
     if (rank == 0)
        printf ("Error, number of processes must be %d for %d x %d  divided matrix transposition of %d x %d \n", b,n,m,n,n);

      MPI_Finalize ();
      return 1;
    }

  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      inputMatrix[i][j] = i + n*(j+1)*(rank);


  for(int i =0; i < n; i++){

          for(int j = 0; j < m; j++){

                printf("A{%d}==>%d<==[%i, %i]\n", rank, inputMatrix[i][j], i, j);
          }

  }


  MPI_Alltoall (&inputMatrix[0][0],m * m,MPI_INT,&outputMatrix[0][0],m * m, MPI_INT, MPI_COMM_WORLD);

  localTrans(outputMatrix);

  for(int i =0; i < n; i++){

          for(int j = 0; j < m; j++){

                printf("B{%d}==>%d<==[%i, %i]\n", rank, outputMatrix[i][j], i, j);
          }

  }


  if (rank == 0)
    printf ("Transpose seems ok\n");

  MPI_Finalize ();

return 0;
}

