#!/bin/bash
#SBATCH -N 6
#SBATCH -J sudoku_mpi_bench
#SBATCH -p short
#SBATCH --ntasks=6                      # Number of MPI ranks
#SBATCH --cpus-per-task=2               # Number of cores per MPI rank
#SBATCH --ntasks-per-node=2             # How many tasks on each node
#SBATCH --ntasks-per-socket=2           # How many tasks on each CPU or socket
#SBATCH --mem-per-cpu=200mb             # Memory per core
#SBATCH --time=00:20:00                 # Time limit hrs:min:sec
#SBATCH --output=sudoku_bench_%j.log     # Standard output and error log

date
echo ""
echo "sudoku_mpi_bench_job.sh"
echo "  Compile and run an MPI program."

module load openmpi/4.0.5.1-gnu

#load gnuplot package for graph generation, might not work if not installed prior
spack load gnuplot

#run the sudoku MPI benchmark 
bash ./run_sudoku_mpi_tests 20

date
echo "  Normal end of execution."
exit 0
