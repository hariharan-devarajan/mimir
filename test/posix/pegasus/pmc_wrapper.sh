#!/bin/bash
export SHM_PATH="/dev/shm/haridev/mimir"
export PFS_PATH="${HOME}/temp/mimir/pfs"
LAUNCH_DIR=`pwd`
echo "Job Launched in directory $LAUNCH_DIR"
pushd $PEGASUS_SCRATCH_DIR
cp $_CONDOR_SCRATCH_DIR/*.in .
mpi_exec=`which mpirun`
PEGASUS_DIR=`which pegasus-mpi-cluster`
echo "${mpi_exec} -env LD_PRELOAD $LD_PRELOAD2 -n 4 ${PEGASUS_DIR} -v $@"
${mpi_exec} -env LD_PRELOAD $LD_PRELOAD2 -n 4 ${PEGASUS_DIR} -v $@
