# elen4020-Project

A Big Data Project on Matrix Transposition using Message Passing Interface(MPI)

# NOTE 

## every output file after transposition has one extra gabbage value at position 0

#### To run matrix transposition mpicc -o mpiMatroxTranspose mpiMatrixTranspose.c
#### To compile  first run mpiMatrixGenerate.c to generate the input files,
#### then mpiexec -n numprocessors ./mpiMatroxTranspose inputfile outputfile
