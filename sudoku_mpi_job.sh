#!/bin/bash
#SBATCH -N 6
#SBATCH -J sudoku_mpi
#SBATCH -p short
#SBATCH --ntasks=6                      # Number of MPI ranks
#SBATCH --cpus-per-task=2               # Number of cores per MPI rank 
#SBATCH --ntasks-per-node=2             # How many tasks on each node
#SBATCH --ntasks-per-socket=2           # How many tasks on each CPU or socket
#SBATCH --mem-per-cpu=200mb             # Memory per core
#SBATCH --time=00:20:00                 # Time limit hrs:min:sec
#SBATCH --output=sudoku_test_%j.log     # Standard output and error log

date
echo ""
echo "sudoku_mpi_job.sh"
echo "  Compile and run an MPI program."

module load openmpi/4.0.5.1-gnu

make mpi
 
mpirun -np 6 ./bin/sudoku_mpi -v "diabolical.txt" "00015097c6c3"
 
date
echo "  Normal end of execution."
exit 0
