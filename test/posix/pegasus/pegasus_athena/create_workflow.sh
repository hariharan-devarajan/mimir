#!/bin/bash

cp ../workflow.py .
cp ../pmc_wrapper.sh .
export ATHENA_LIB_PATH=/home/hariharan/CLionProjects/mimir/build/libathena.so
python workflow.py --mimir_bin /home/hariharan/CLionProjects/mimir/build/test/posix/ --pfs /home/hariharan/temp/mimir/pfs --shm /home/hariharan/temp/mimir/shm --jobs 16 --intercept --pmc

pegasus-plan --dir work --dax workflow.yml --output-site local --cluster whole
